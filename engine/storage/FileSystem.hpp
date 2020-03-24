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
#include "storage/Archive.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    class Engine;

    namespace storage
    {
        class FileSystem final
        {
        public:
            explicit FileSystem(Engine& initEngine);

            std::string getStorageDirectory(const bool user = true) const;

            static Path getTempPath()
            {
#if defined(_WIN32)
                WCHAR buffer[MAX_PATH];
                if (!GetTempPathW(MAX_PATH, buffer))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get temp directory");

                return Path{buffer, Path::Format::Native};
#elif defined(__linux__) || defined(__APPLE__)
                char const* path = getenv("TMPDIR");
                if (!path) path = getenv("TMP");
                if (!path) path = getenv("TEMP");
                if (!path) path = getenv("TEMPDIR");

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

            std::vector<std::uint8_t> readFile(const std::string& filename, const bool searchResources = true);

            bool resourceFileExists(const std::string& filename) const;

            std::string getPath(const std::string& filename, const bool searchResources = true) const
            {
                if (Path(filename).isAbsolute())
                {
                    if (fileExists(filename))
                        return filename;
                }
                else
                {
                    std::string str = appPath + Path::directorySeparator + filename;

                    if (fileExists(str))
                        return str;

                    if (searchResources)
                        for (const std::string& path : resourcePaths)
                        {
                            if (Path(path).isAbsolute()) // if resource path is absolute
                                str = path + Path::directorySeparator + filename;
                            else
                                str = appPath + Path::directorySeparator + path + Path::directorySeparator + filename;

                            if (fileExists(str))
                                return str;
                        }
                }

                throw std::runtime_error("Could not get path for " + filename);
            }

            void addResourcePath(const std::string& path)
            {
                const auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

                if (i == resourcePaths.end())
                    resourcePaths.push_back(path);
            }

            void removeResourcePath(const std::string& path)
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

            bool directoryExists(const std::string& dirname) const;
            bool fileExists(const std::string& filename) const;

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
                mode_t mode = 0777;
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
                    FileDescriptor() noexcept = default;
                    ~FileDescriptor() { if (fd != -1) close(fd); }
                    FileDescriptor(const FileDescriptor&) = delete;
                    FileDescriptor& operator=(const FileDescriptor&) = delete;
                    FileDescriptor& operator=(int f) noexcept
                    {
                        if (fd != -1) close(fd);
                        fd = f;
                        return *this;
                    }
                    operator int() const noexcept { return fd; }
                private:
                    int fd = -1;
                };

                FileDescriptor in;
                if ((in = open(from.getNative().c_str(), O_RDONLY)) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to open file");

                const int mode = O_CREAT | O_WRONLY | O_TRUNC | (!overwrite ? O_EXCL : 0);

                struct stat s;
                if (fstat(in, &s) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to get file stats");

                FileDescriptor out;
                if ((out = open(to.getNative().c_str(), mode, s.st_mode)) == -1)
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

            void deleteFile(const Path& path)
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

        private:
            Engine& engine;
            std::string appPath;
            std::vector<std::string> resourcePaths;
            std::vector<std::pair<std::string, Archive>> archives;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_FILESYSTEM_HPP
