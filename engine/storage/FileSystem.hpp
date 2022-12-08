// Ouzel by Elviss Strazdins

#ifndef OUZEL_STORAGE_FILESYSTEM_HPP
#define OUZEL_STORAGE_FILESYSTEM_HPP

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <vector>
#ifdef _WIN32
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  include <ShlObj.h>
#  include <Shlwapi.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__unix__) || defined(__APPLE__)
#  include <limits.h>
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif
#include "Archive.hpp"
#include "Path.hpp"
#include "StorageError.hpp"

namespace ouzel::core
{
    class Engine;
}

namespace ouzel::storage
{
    enum class FileType
    {
        notFound,
        regular,
        directory,
        symlink,
        block,
        character,
        fifo,
        socket,
        unknown
    };

    enum class Permissions
    {
        none = 0,
        ownerRead = 0400,
        ownerWrite = 0200,
        ownerExecute = 0100,
        ownerAll = 0700,
        groupRead = 040,
        groupWrite = 020,
        groupExecute = 010,
        groupAll = 070,
        othersRead = 04,
        othersWrite = 02,
        othersExecute = 01,
        othersAll = 07,
        all = 0777
    };

    [[nodiscard]] inline constexpr Permissions operator&(const Permissions a, const Permissions b) noexcept
    {
        return static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) & static_cast<std::underlying_type_t<Permissions>>(b));
    }
    [[nodiscard]] inline constexpr Permissions operator|(const Permissions a, const Permissions b) noexcept
    {
        return static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) | static_cast<std::underlying_type_t<Permissions>>(b));
    }
    [[nodiscard]] inline constexpr Permissions operator^(const Permissions a, const Permissions b) noexcept
    {
        return static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) ^ static_cast<std::underlying_type_t<Permissions>>(b));
    }
    [[nodiscard]] inline constexpr Permissions operator~(const Permissions a) noexcept
    {
        return static_cast<Permissions>(~static_cast<std::underlying_type_t<Permissions>>(a));
    }
    inline constexpr Permissions& operator&=(Permissions& a, const Permissions b) noexcept
    {
        return a = static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) & static_cast<std::underlying_type_t<Permissions>>(b));
    }
    inline constexpr Permissions& operator|=(Permissions& a, const Permissions b) noexcept
    {
        return a = static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) | static_cast<std::underlying_type_t<Permissions>>(b));
    }
    inline constexpr Permissions& operator^=(Permissions& a, const Permissions b) noexcept
    {
        return a = static_cast<Permissions>(static_cast<std::underlying_type_t<Permissions>>(a) ^ static_cast<std::underlying_type_t<Permissions>>(b));
    }

    class FileTime final
    {
    public:
#ifdef _WIN32
        using Type = FILETIME;
#elif defined(__unix__) || defined(__APPLE__)
        using Type = timespec;
#else
#  error "Unsupported platform"
#endif
        FileTime() noexcept = default;
        FileTime(const Type& t) noexcept: time{t} {}

        operator Type() const noexcept { return time; }

        [[nodiscard]] operator std::chrono::system_clock::time_point() const noexcept
        {
#ifdef _WIN32
            using hundrednanoseconds = std::chrono::duration<std::int64_t, std::ratio_multiply<std::hecto, std::nano>>;

            const auto t = hundrednanoseconds{
                ((static_cast<std::uint64_t>(time.dwHighDateTime) << 32) |
                 static_cast<std::uint64_t>(time.dwLowDateTime)) - 116444736000000000LL
            };

            return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(t)};
#elif defined(__unix__) || defined(__APPLE__)
            const auto nanoseconds = std::chrono::seconds{time.tv_sec} +
                std::chrono::nanoseconds{time.tv_nsec};

            return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)};
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] bool operator==(const FileTime& other) const noexcept
        {
#ifdef _WIN32
            return time.dwHighDateTime == other.time.dwHighDateTime &&
                time.dwLowDateTime == other.time.dwLowDateTime;
#elif defined(__unix__) || defined(__APPLE__)
            return time.tv_sec == other.time.tv_sec &&
                time.tv_nsec == other.time.tv_nsec;
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] bool operator>(const FileTime& other) const noexcept
        {
#ifdef _WIN32
            return time.dwHighDateTime == other.time.dwHighDateTime ?
                time.dwLowDateTime > other.time.dwLowDateTime :
                time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
            return time.tv_sec == other.time.tv_sec ?
                time.tv_nsec > other.time.tv_nsec :
                time.tv_sec > other.time.tv_sec;
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] bool operator<(const FileTime& other) const noexcept
        {
#ifdef _WIN32
            return time.dwHighDateTime == other.time.dwHighDateTime ?
                time.dwLowDateTime < other.time.dwLowDateTime :
                time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
            return time.tv_sec == other.time.tv_sec ?
                time.tv_nsec < other.time.tv_nsec :
                time.tv_sec < other.time.tv_sec;
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] bool operator>=(const FileTime& other) const noexcept
        {
#ifdef _WIN32
            return time.dwHighDateTime == other.time.dwHighDateTime ?
                time.dwLowDateTime >= other.time.dwLowDateTime :
                time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
            return time.tv_sec == other.time.tv_sec ?
                time.tv_nsec >= other.time.tv_nsec :
                time.tv_sec > other.time.tv_sec;
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] bool operator<=(const FileTime& other) const noexcept
        {
#ifdef _WIN32
            return time.dwHighDateTime == other.time.dwHighDateTime ?
                time.dwLowDateTime <= other.time.dwLowDateTime :
                time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
            return time.tv_sec == other.time.tv_sec ?
                time.tv_nsec <= other.time.tv_nsec :
                time.tv_sec < other.time.tv_sec;
#else
#  error "Unsupported platform"
#endif
        }

    private:
        Type time;
    };

    class FileSystem final
    {
    public:
        explicit FileSystem(core::Engine& initEngine);

        Path getStorageDirectory(const bool user = true) const;

        [[nodiscard]] static Path getTempPath()
        {
#ifdef _WIN32
            WCHAR buffer[MAX_PATH + 1];
            if (!GetTempPathW(MAX_PATH + 1, buffer))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get temp directory"};

            return Path{buffer, Path::Format::native};
#elif defined(__linux__) || defined(__APPLE__)
            auto path = std::getenv("TMPDIR");
            if (!path) path = std::getenv("TMP");
            if (!path) path = std::getenv("TEMP");
            if (!path) path = std::getenv("TEMPDIR");

            if (path)
                return Path{path, Path::Format::native};
            else
#  ifdef __ANDROID__
                return Path{"/data/local/tmp", Path::Format::native};
#  else
                return Path{"/tmp", Path::Format::native};
#  endif
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] std::vector<std::byte> readFile(const Path& filename, const bool searchResources = true);

        bool resourceFileExists(const Path& filename) const;

        [[nodiscard]] Path getPath(const Path& filename, const bool searchResources = true) const
        {
            if (filename.isAbsolute())
            {
                if (fileExists(filename))
                    return filename;
            }
            else
            {
                auto result = appPath / filename;

                if (fileExists(result))
                    return result;

                if (searchResources)
                    for (const auto& path : resourcePaths)
                    {
                        if (path.isAbsolute()) // if resource path is absolute
                            result = path / filename;
                        else
                            result = appPath / path / filename;

                        if (fileExists(result))
                            return result;
                    }
            }

            throw Error{"Could not get path for " + std::string(filename)};
        }

        void addResourcePath(const Path& path)
        {
            if (std::find(resourcePaths.begin(), resourcePaths.end(), path) == resourcePaths.end())
                resourcePaths.push_back(path);
        }

        void removeResourcePath(const Path& path)
        {
            if (const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path); i != resourcePaths.end())
                resourcePaths.erase(i);
        }

        void addArchive(const std::string& name, Archive&& archive)
        {
            archives.emplace_back(name, std::move(archive));
        }

        void removeArchive(const std::string& name)
        {
            for (auto i = archives.begin(); i != archives.end();)
                if (i->first == name)
                    i = archives.erase(i);
                else
                    ++i;
        }

        [[nodiscard]] bool directoryExists(const Path& dirname) const;
        [[nodiscard]] bool fileExists(const Path& filename) const;

        [[nodiscard]] static Path getCurrentPath()
        {
#ifdef _WIN32
            const auto pathLength = GetCurrentDirectoryW(0, 0);
            auto buffer = std::make_unique<wchar_t[]>(pathLength);
            if (GetCurrentDirectoryW(pathLength, buffer.get()) == 0)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get current directory"};
            return Path{buffer.get(), Path::Format::native};
#elif defined(__unix__) || defined(__APPLE__)
            const auto pathMaxConfig = pathconf(".", _PC_PATH_MAX);
            const auto pathMax = static_cast<std::size_t>(pathMaxConfig == -1 ? PATH_MAX : pathMaxConfig);
            auto buffer = std::make_unique<char[]>(pathMax + 1);
            if (!getcwd(buffer.get(), pathMax))
                throw std::system_error{errno, std::system_category(), "Failed to get current directory"};
            return Path{buffer.get(), Path::Format::native};
#else
#  error "Unsupported platform"
#endif
        }

        static void setCurrentPath(const Path& path)
        {
#ifdef _WIN32
            if (SetCurrentDirectoryW(path.getNative().c_str()) == 0)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to set current directory"};
#elif defined(__unix__) || defined(__APPLE__)
            if (chdir(path.getNative().c_str()) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to set current directory"};
#else
#  error "Unsupported platform"
#endif
        }

        static void createDirectory(const Path& path)
        {
#ifdef _WIN32
            if (CreateDirectoryW(path.getNative().c_str(), nullptr) == 0)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to create directory"};
#elif defined(__unix__) || defined(__APPLE__)
            const mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
            if (mkdir(path.getNative().c_str(), mode) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to create directory"};
#else
#  error "Unsupported platform"
#endif
        }

        static void copyFile(const Path& from, const Path& to, bool overwrite = false)
        {
#ifdef _WIN32
            if (!CopyFileW(from.getNative().c_str(), to.getNative().c_str(), overwrite ? FALSE : TRUE))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to copy file"};
#elif defined(__unix__) || defined(__APPLE__)
            class FileDescriptor final
            {
            public:
                FileDescriptor(int f) noexcept: fd{f} {}
                ~FileDescriptor() { if (fd != -1) close(fd); }
                FileDescriptor(FileDescriptor&& other) noexcept: fd{other.fd}
                {
                    other.fd = -1;
                }
                FileDescriptor& operator=(FileDescriptor&& other) noexcept
                {
                    if (&other == this) return *this;
                    if (fd != -1) close(fd);
                    fd = other.fd;
                    other.fd = -1;
                    return *this;
                }
                auto get() const noexcept { return fd; }
            private:
                int fd = -1;
            };

            auto inFileDescriptor = open(from.getNative().c_str(), O_RDONLY);
            while (inFileDescriptor == -1 && errno == EINTR)
                inFileDescriptor = open(from.getNative().c_str(), O_RDONLY);

            if (inFileDescriptor == -1)
                throw std::system_error{errno, std::system_category(), "Failed to open file"};

            const FileDescriptor in = inFileDescriptor;

            const auto flags = O_CREAT | O_WRONLY | O_TRUNC | (overwrite ? 0 : O_EXCL);

            struct stat s;
            if (fstat(in.get(), &s) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get file status"};

            auto outFileDescriptor = open(to.getNative().c_str(), flags, s.st_mode);
            while (outFileDescriptor == -1 && errno == EINTR)
                outFileDescriptor = open(to.getNative().c_str(), flags, s.st_mode);

            if (outFileDescriptor == -1)
                throw std::system_error{errno, std::system_category(), "Failed to open file"};

            const FileDescriptor out = outFileDescriptor;

            std::vector<char> buffer(16384);
            for (;;)
            {
                auto bytesRead = read(in.get(), buffer.data(), buffer.size());
                while (bytesRead == -1 && errno == EINTR)
                    bytesRead = read(in.get(), buffer.data(), buffer.size());

                if (bytesRead == -1)
                    throw std::system_error{errno, std::system_category(), "Failed to read from file"};
                else
                    if (bytesRead == 0) break;

                ssize_t offset = 0;
                do
                {
                    auto bytesWritten = write(out.get(), buffer.data() + offset, static_cast<size_t>(bytesRead));
                    while (bytesWritten == -1 && errno == EINTR)
                        bytesWritten = write(out.get(), buffer.data() + offset, static_cast<size_t>(bytesRead));

                    if (bytesWritten == -1)
                        throw std::system_error{errno, std::system_category(), "Failed to write to file"};

                    bytesRead -= bytesWritten;
                    offset += bytesWritten;
                } while (bytesRead);
            }
#else
#  error "Unsupported platform"
#endif
        }

        static void renameFile(const Path& from, const Path& to)
        {
#ifdef _WIN32
            if (!MoveFileExW(from.getNative().c_str(), to.getNative().c_str(), MOVEFILE_REPLACE_EXISTING))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to move file"};
#elif defined(__unix__) || defined(__APPLE__)
            if (rename(from.getNative().c_str(), to.getNative().c_str()) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to move file"};
#else
#  error "Unsupported platform"
#endif
        }

        static void deleteFile(const Path& path)
        {
#ifdef _WIN32
            const auto attributes = GetFileAttributesW(path.getNative().c_str());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get file attributes"};

            if (attributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!RemoveDirectoryW(path.getNative().c_str()))
                    throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to delete directory"};
            }
            else if (!DeleteFileW(path.getNative().c_str()))
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to delete file"};

#elif defined(__unix__) || defined(__APPLE__)
            if (remove(path.getNative().c_str()) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to delete file"};
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] static FileType getFileType(const Path& path) noexcept
        {
#ifdef _WIN32
            const auto attributes = GetFileAttributesW(path.getNative().c_str());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                return FileType::notFound;

            if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                return FileType::symlink;
            else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                return FileType::directory;
            else
                return FileType::regular;
#elif defined(__unix__) || defined(__APPLE__)
            struct stat s;
            if (lstat(path.getNative().c_str(), &s) == -1)
                return FileType::notFound;

            if ((s.st_mode & S_IFMT) == S_IFREG)
                return FileType::regular;
            else if ((s.st_mode & S_IFMT) == S_IFDIR)
                return FileType::directory;
            else if ((s.st_mode & S_IFMT) == S_IFLNK)
                return FileType::symlink;
            else if ((s.st_mode & S_IFMT) == S_IFBLK)
                return FileType::block;
            else if ((s.st_mode & S_IFMT) == S_IFCHR)
                return FileType::character;
            else if ((s.st_mode & S_IFMT) == S_IFIFO)
                return FileType::fifo;
            else if ((s.st_mode & S_IFMT) == S_IFSOCK)
                return FileType::socket;
            else
                return FileType::unknown;
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] static std::size_t getFileSize(const Path& path)
        {
#ifdef _WIN32
            WIN32_FILE_ATTRIBUTE_DATA attributes;
            if (!GetFileAttributesExW(path.getNative().c_str(), GetFileExInfoStandard, &attributes))
                throw std::system_error{errno, std::system_category(), "Failed to get file attributes"};
            const auto fileSize = static_cast<std::uint64_t>(attributes.nFileSizeHigh) << (sizeof(attributes.nFileSizeLow) * 8) | attributes.nFileSizeLow;
            return static_cast<std::size_t>(fileSize);
#elif defined(__unix__) || defined(__APPLE__)
            struct stat s;
            if (lstat(path.getNative().c_str(), &s) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get file status"};
            return static_cast<std::size_t>(s.st_size);
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] static Permissions getPermissions(const Path& path)
        {
#ifdef _WIN32
            const auto attributes = GetFileAttributesW(path.getNative().c_str());
            if (attributes == INVALID_FILE_ATTRIBUTES)
                throw std::system_error{errno, std::system_category(), "Failed to get file attributes"};

            return (attributes & FILE_ATTRIBUTE_READONLY) ?
                Permissions::ownerRead | Permissions::groupRead | Permissions::othersRead |
                Permissions::ownerExecute | Permissions::groupExecute | Permissions::othersExecute :
                Permissions::all;

#elif defined(__unix__) || defined(__APPLE__)
            struct stat s;
            if (lstat(path.getNative().c_str(), &s) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get file status"};
            return static_cast<Permissions>(s.st_mode);
#else
#  error "Unsupported platform"
#endif
        }

        static void setPermissions(const Path& path, Permissions permissions)
        {
#ifdef _WIN32
            const auto attributes = (permissions & Permissions::ownerWrite) == Permissions::ownerWrite ?
                FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_READONLY;
            if (!SetFileAttributesW(path.getNative().c_str(), attributes))
                throw std::system_error{errno, std::system_category(), "Failed to set file attributes"};
#elif defined(__unix__) || defined(__APPLE__)
            const auto mode = static_cast<mode_t>(permissions);
            if (chmod(path.getNative().c_str(), mode) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to set file permissions"};
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] static FileTime getAccessTime(const Path& path)
        {
#ifdef _WIN32
            HANDLE file = CreateFileW(path.getNative().c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to open file"};

            FILETIME time;
            const auto ret = GetFileTime(file, nullptr, &time, nullptr);
            CloseHandle(file);
            if (!ret)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get file time"};

            return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
            struct stat s;
            if (lstat(path.getNative().c_str(), &s) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get file status"};

#  ifdef __APPLE__
            return FileTime{s.st_atimespec};
#  else
            return FileTime{s.st_atim};
#  endif
#else
#  error "Unsupported platform"
#endif
        }

        [[nodiscard]] static FileTime getModifyTime(const Path& path)
        {
#ifdef _WIN32
            HANDLE file = CreateFileW(path.getNative().c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to open file"};

            FILETIME time;
            const auto ret = GetFileTime(file, nullptr, nullptr, &time);
            CloseHandle(file);
            if (!ret)
                throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get file time"};

            return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
            struct stat s;
            if (lstat(path.getNative().c_str(), &s) == -1)
                throw std::system_error{errno, std::system_category(), "Failed to get file status"};

#  ifdef __APPLE__
            return FileTime{s.st_mtimespec};
#  else
            return FileTime{s.st_mtim};
#  endif
#else
#  error "Unsupported platform"
#endif
        }

    private:
        core::Engine& engine;
        Path appPath;
        std::vector<Path> resourcePaths;
        std::vector<std::pair<std::string, Archive>> archives;
    };
}

#endif // OUZEL_STORAGE_FILESYSTEM_HPP
