// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_FILESYSTEM_HPP
#define OUZEL_STORAGE_FILESYSTEM_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#if defined(_WIN32)
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
#  include <sys/fcntl.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif
#include "Archive.hpp"
#include "Path.hpp"

namespace ouzel
{
    class Engine;

    namespace storage
    {
        enum class FileType
        {
            NotFound,
            Regular,
            Directory,
            Symlink,
            Block,
            Character,
            Fifo,
            Socket,
            Unknown
        };

        class FileTime final
        {
        public:
#if defined(_WIN32)
            using Type = FILETIME;
#elif defined(__unix__) || defined(__APPLE__)
            using Type = timespec;
#endif
            FileTime() noexcept = default;
            FileTime(const Type& t) noexcept: time{t} {}

            operator Type() const noexcept { return time; }

            operator std::chrono::system_clock::time_point() const noexcept
            {
#if defined(_WIN32)
                using hundrednanoseconds = std::chrono::duration<std::int64_t, std::ratio_multiply<std::hecto, std::nano>>;

                auto t = hundrednanoseconds{
                    ((static_cast<std::uint64_t>(time.dwHighDateTime) << 32) |
                     static_cast<std::uint64_t>(time.dwLowDateTime)) - 116444736000000000LL
                };

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(t)};
#elif defined(__unix__) || defined(__APPLE__)
                auto nanoseconds = std::chrono::seconds{time.tv_sec} +
                    std::chrono::nanoseconds{time.tv_nsec};

                return std::chrono::system_clock::time_point{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)};
#endif
            }

            bool operator==(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime &&
                    time.dwLowDateTime == other.time.dwLowDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec &&
                    time.tv_nsec == other.time.tv_nsec;
#endif
            }

            bool operator>(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime > other.time.dwLowDateTime :
                    time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec > other.time.tv_nsec :
                    time.tv_sec > other.time.tv_sec;
#endif
            }

            bool operator<(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime < other.time.dwLowDateTime :
                    time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec < other.time.tv_nsec :
                    time.tv_sec < other.time.tv_sec;
#endif
            }

            bool operator>=(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime >= other.time.dwLowDateTime :
                    time.dwHighDateTime > other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec >= other.time.tv_nsec :
                    time.tv_sec > other.time.tv_sec;
#endif
            }

            bool operator<=(const FileTime& other) const noexcept
            {
#if defined(_WIN32)
                return time.dwHighDateTime == other.time.dwHighDateTime ?
                    time.dwLowDateTime <= other.time.dwLowDateTime :
                    time.dwHighDateTime < other.time.dwHighDateTime;
#elif defined(__unix__) || defined(__APPLE__)
                return time.tv_sec == other.time.tv_sec ?
                    time.tv_nsec <= other.time.tv_nsec :
                    time.tv_sec < other.time.tv_sec;
#endif
            }

        private:
            Type time;
        };

        class FileSystem final
        {
        public:
            explicit FileSystem(Engine& initEngine);

            Path getStorageDirectory(const bool user = true) const;

            static Path getTempPath()
            {
#if defined(_WIN32)
                WCHAR buffer[MAX_PATH + 1];
                if (!GetTempPathW(MAX_PATH + 1, buffer))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get temp directory");

                return Path{buffer, Path::Format::Native};
#elif defined(__linux__) || defined(__APPLE__)
                char const* path = std::getenv("TMPDIR");
                if (!path) path = std::getenv("TMP");
                if (!path) path = std::getenv("TEMP");
                if (!path) path = std::getenv("TEMPDIR");

                if (path)
                    return Path{path, Path::Format::Native};
                else
#  if defined(__ANDROID__)
                    return Path{"/data/local/tmp", Path::Format::Native};
#  else
                    return Path{"/tmp", Path::Format::Native};
#  endif
#endif
            }

            std::vector<std::uint8_t> readFile(const Path& filename, const bool searchResources = true);

            bool resourceFileExists(const Path& filename) const;

            Path getPath(const Path& filename, const bool searchResources = true) const
            {
                if (filename.isAbsolute())
                {
                    if (fileExists(filename))
                        return filename;
                }
                else
                {
                    Path result = appPath / filename;

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

                throw std::runtime_error("Could not get path for " + std::string(filename));
            }

            void addResourcePath(const Path& path)
            {
                const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

                if (i == resourcePaths.end())
                    resourcePaths.push_back(path);
            }

            void removeResourcePath(const Path& path)
            {
                const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

                if (i != resourcePaths.end())
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

            bool directoryExists(const Path& dirname) const;
            bool fileExists(const Path& filename) const;

            static Path getCurrentPath()
            {
#if defined(_WIN32)
                DWORD pathLength = GetCurrentDirectoryW(0, 0);
                std::unique_ptr<wchar_t[]> buffer(new wchar_t[pathLength]);
                if (GetCurrentDirectoryW(pathLength, buffer.get()) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get current directory");
                return Path{buffer.get(), Path::Format::Native};
#elif defined(__unix__) || defined(__APPLE__)
                const auto pathMaxConfig = pathconf(".", _PC_PATH_MAX);
                const size_t pathMax = static_cast<size_t>(pathMaxConfig == -1 ? PATH_MAX : pathMaxConfig);
                std::unique_ptr<char[]> buffer(new char[pathMax + 1]);
                if (!getcwd(buffer.get(), pathMax))
                    throw std::system_error(errno, std::system_category(), "Failed to get current directory");
                return Path{buffer.get(), Path::Format::Native};
#endif
            }

            static void setCurrentPath(const Path& path)
            {
#if defined(_WIN32)
                if (SetCurrentDirectoryW(path.getNative().c_str()) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set current directory");
#elif defined(__unix__) || defined(__APPLE__)
                if (chdir(path.getNative().c_str()) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to set current directory");
#endif
            }

            static void createDirectory(const Path& path)
            {
#if defined(_WIN32)
                if (CreateDirectoryW(path.getNative().c_str(), nullptr) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to create directory");
#elif defined(__unix__) || defined(__APPLE__)
                const mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
                if (mkdir(path.getNative().c_str(), mode) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to create directory");
#endif
            }

            static void copyFile(const Path& from, const Path& to, bool overwrite = false)
            {
#if defined(_WIN32)
                if (!CopyFileW(from.getNative().c_str(), to.getNative().c_str(), !overwrite))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to copy file");
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
                        if (this == &other) return *this;
                        if (fd != -1) close(fd);
                        fd = other.fd;
                        other.fd = -1;
                        return *this;
                    }
                    operator int() const noexcept { return fd; }
                private:
                    int fd = -1;
                };

                const FileDescriptor in = open(from.getNative().c_str(), O_RDONLY);
                if (in == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to open file");

                const int flags = O_CREAT | O_WRONLY | O_TRUNC | (overwrite ? 0 : O_EXCL);

                struct stat s;
                if (fstat(in, &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file status");

                const FileDescriptor out = open(to.getNative().c_str(), flags, s.st_mode);
                if (out == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to open file");

                std::vector<char> buffer(16384);
                for (;;)
                {
                    ssize_t bytesRead = read(in, buffer.data(), buffer.size());
                    if (bytesRead == -1)
                        throw std::system_error(errno, std::system_category(), "Failed to read from file");

                    if (bytesRead == 0) break;

                    ssize_t offset = 0;
                    do
                    {
                        const ssize_t bytesWritten = write(out, buffer.data() + offset, static_cast<size_t>(bytesRead));
                        if (bytesWritten == -1)
                            throw std::system_error(errno, std::system_category(), "Failed to write to file");

                        bytesRead -= bytesWritten;
                        offset += bytesWritten;
                    } while (bytesRead);
                }
#endif
            }

            static void moveFile(const Path& from, const Path& to)
            {
#if defined(_WIN32)
                if (!MoveFileExW(from.getNative().c_str(), to.getNative().c_str(), MOVEFILE_REPLACE_EXISTING))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to move file");
#elif defined(__unix__) || defined(__APPLE__)
                if (rename(from.getNative().c_str(), to.getNative().c_str()) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to move file");
#endif
            }

            static void deleteFile(const Path& path)
            {
#if defined(_WIN32)
                const DWORD attributes = GetFileAttributesW(path.getNative().c_str());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file attributes");

                if (attributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (!RemoveDirectoryW(path.getNative().c_str()))
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to delete directory");
                }
                else if (!DeleteFileW(path.getNative().c_str()))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to delete file");

#elif defined(__unix__) || defined(__APPLE__)
                if (remove(path.getNative().c_str()) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to delete file");
#endif
            }

            static FileType getFileType(const Path& path) noexcept
            {
#if defined(_WIN32)
                const DWORD attributes = GetFileAttributesW(path.getNative().c_str());
                if (attributes == INVALID_FILE_ATTRIBUTES)
                    return FileType::NotFound;

                if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
                    return FileType::Symlink;
                else if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                    return FileType::Directory;
                else
                    return FileType::Regular;
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (stat(path.getNative().c_str(), &s) == -1)
                    return FileType::NotFound;

                if ((s.st_mode & S_IFMT) == S_IFREG)
                    return FileType::Regular;
                else if ((s.st_mode & S_IFMT) == S_IFDIR)
                    return FileType::Directory;
                else if ((s.st_mode & S_IFMT) == S_IFLNK)
                    return FileType::Symlink;
                else if ((s.st_mode & S_IFMT) == S_IFBLK)
                    return FileType::Block;
                else if ((s.st_mode & S_IFMT) == S_IFCHR)
                    return FileType::Character;
                else if ((s.st_mode & S_IFMT) == S_IFIFO)
                    return FileType::Fifo;
                else if ((s.st_mode & S_IFMT) == S_IFSOCK)
                    return FileType::Socket;
                else
                    return FileType::Unknown;
#endif
            }

            static bool isFile(const Path& path) noexcept
            {
                return getFileType(path) != FileType::NotFound;
            }

            static bool isDirectory(const Path& path) noexcept
            {
                return getFileType(path) == FileType::Directory;
            }

            static bool isRegular(const Path& path) noexcept
            {
                return getFileType(path) == FileType::Regular;
            }

            static size_t getFileSize(const Path& path)
            {
#if defined(_WIN32)
                WIN32_FILE_ATTRIBUTE_DATA attributes;
                if (!GetFileAttributesExW(path.getNative().c_str(), GetFileExInfoStandard, &attributes))
                    throw std::system_error(errno, std::system_category(), "Failed to get file attributes");
                return static_cast<size_t>(attr.nFileSizeHigh) << (sizeof(attr.nFileSizeHigh) * 8) | attr.nFileSizeLow;
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (stat(path.getNative().c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file status");
                return static_cast<size_t>(s.st_size);
#endif
            }

            static FileTime getAccessTime(const Path& path)
            {
#if defined(_WIN32)
                HANDLE file = CreateFileW(path.getNative().c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, &time, nullptr);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (lstat(path.getNative().c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file status");

#  if defined(__APPLE__)
                return FileTime{s.st_atimespec};
#  else
                return FileTime{s.st_atim};
#  endif
#endif
            }

            static FileTime getModifyTime(const Path& path)
            {
#if defined(_WIN32)
                HANDLE file = CreateFileW(path.getNative().c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (file == INVALID_HANDLE_VALUE)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to open file");

                FILETIME time;
                auto ret = GetFileTime(file, nullptr, nullptr, &time);
                CloseHandle(file);
                if (!ret)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get file time");

                return FileTime{time};
#elif defined(__unix__) || defined(__APPLE__)
                struct stat s;
                if (lstat(path.getNative().c_str(), &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file status");

#  if defined(__APPLE__)
                return FileTime{s.st_mtimespec};
#  else
                return FileTime{s.st_mtim};
#  endif
#endif
            }

        private:
            Engine& engine;
            Path appPath;
            std::vector<Path> resourcePaths;
            std::vector<std::pair<std::string, Archive>> archives;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_FILESYSTEM_HPP
