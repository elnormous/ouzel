// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#if OUZEL_PLATFORM_WINDOWS
#include <Windows.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <CoreFoundation/CoreFoundation.h>
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
#include <unistd.h>
#elif OUZEL_PLATFORM_ANDROID
#include "core/android/EngineAndroid.hpp"
#endif

#include "FileSystem.hpp"
#include "Archive.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
#if OUZEL_PLATFORM_WINDOWS
    const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
    const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif

    FileSystem::FileSystem()
    {
#if OUZEL_PLATFORM_WINDOWS
        char* exePath = _pgmptr;

        if (exePath)
        {
            appPath = getDirectoryPart(exePath);
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        CFBundleRef bundle = CFBundleGetMainBundle(); // [NSBundle mainBundle]
        CFURLRef path = CFBundleCopyResourcesDirectoryURL(bundle); // [bundle resourceURL]

        if (path)
        {
            char resourceDirectory[1024];
            CFURLGetFileSystemRepresentation(path, true, reinterpret_cast<UInt8*>(resourceDirectory), sizeof(resourceDirectory));
            CFRelease(path);
            appPath = resourceDirectory;
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get resource directory";
        }
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        char executableDirectory[1024];

        if (readlink("/proc/self/exe", executableDirectory, sizeof(executableDirectory)) != -1)
        {
            appPath = getDirectoryPart(executableDirectory);
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
#endif
    }

    std::string FileSystem::getStorageDirectory(bool) const
    {
        return "";
    }

    std::string FileSystem::getTempDirectory() const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        char tempDirectory[1024];
        if (GetTempPathW(MAX_PATH, szBuffer))
        {
            if (WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, tempDirectory, sizeof(tempDirectory), nullptr, nullptr) == 0)
            {
                Log(Log::Level::ERR) << "Failed to convert UTF-8 to wide char";
                return "";
            }

            return tempDirectory;
        }
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        char tempDirectory[1024];
        size_t n = confstr(_CS_DARWIN_USER_TEMP_DIR, tempDirectory, sizeof(tempDirectory));
        if ((n <= 0) || (n >= sizeof(tempDirectory)))
            strlcpy(tempDirectory, getenv("TMPDIR"), sizeof(tempDirectory));
        CFURLRef tmp = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, reinterpret_cast<UInt8*>(tempDirectory), static_cast<CFIndex>(strlen(tempDirectory)), true);

        CFURLGetFileSystemRepresentation(tmp, true, reinterpret_cast<UInt8*>(tempDirectory), sizeof(tempDirectory));
        CFRelease(tmp);

        return tempDirectory;
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        char const* path = getenv("TMPDIR");
        if (path)
            return path;
        else
            return "/tmp";
#elif OUZEL_PLATFORM_ANDROID
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        return engineAndroid->getCacheDirectory();
#else
        return "";
#endif
    }

    bool FileSystem::readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources) const
    {
        if (searchResources)
        {
            for (const auto& archive : archives)
            {
                if (archive->readFile(filename, data))
                {
                    return true;
                }
            }
        }

        std::string path = getPath(filename, searchResources);

        // file does not exist
        if (path.empty())
        {
            Log(Log::Level::ERR) << "Failed to find file " << filename;
            return false;
        }

        std::ifstream file(path, std::ios::binary);

        if (!file)
        {
            Log(Log::Level::ERR) << "Failed to open file " << path;
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        return true;
    }

    bool FileSystem::writeFile(const std::string& filename, const std::vector<uint8_t>& data) const
    {
        std::ofstream file(filename, std::ios::binary);

        if (!file)
        {
            Log(Log::Level::ERR) << "Failed to open file " << filename;
            return false;
        }

        file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

        return true;
    }

    bool FileSystem::resourceFileExists(const std::string& filename) const
    {
        if (isAbsolutePath(filename))
        {
            return fileExists(filename);
        }
        else
        {
            std::string str = appPath + DIRECTORY_SEPARATOR + filename;

            if (fileExists(str))
            {
                return true;
            }
            else
            {
                for (const std::string& path : resourcePaths)
                {
                    if (isAbsolutePath(path)) // if resource path is absolute
                    {
                        str = path + DIRECTORY_SEPARATOR + filename;
                    }
                    else
                    {
                        str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;
                    }

                    if (fileExists(str))
                    {
                        return true;
                    }
                }
            }

            return false;
        }
    }

    bool FileSystem::directoryExists(const std::string& dirname) const
    {
        struct stat buf;
        if (stat(dirname.c_str(), &buf) != 0)
        {
            return false;
        }

        return (buf.st_mode & S_IFMT) == S_IFDIR;
    }

    bool FileSystem::fileExists(const std::string& filename) const
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != 0)
        {
            return false;
        }

        return (buf.st_mode & S_IFMT) == S_IFREG;
    }

    std::string FileSystem::getPath(const std::string& filename, bool searchResources) const
    {
        if (isAbsolutePath(filename))
        {
            if (fileExists(filename))
            {
                return filename;
            }
        }
        else
        {
            std::string str = appPath + DIRECTORY_SEPARATOR + filename;

            if (fileExists(str))
            {
                return str;
            }

            if (searchResources)
            {
                for (const std::string& path : resourcePaths)
                {
                    if (isAbsolutePath(path)) // if resource path is absolute
                    {
                        str = path + DIRECTORY_SEPARATOR + filename;
                    }
                    else
                    {
                        str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;
                    }

                    if (fileExists(str))
                    {
                        return str;
                    }
                }
            }
        }

        return "";
    }

    void FileSystem::addResourcePath(const std::string& path)
    {
        auto i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

        if (i == resourcePaths.end())
        {
            resourcePaths.push_back(path);
        }
    }

    void FileSystem::addArchive(Archive* archive)
    {
        if (archive->fileSystem) archive->fileSystem->removeArchive(archive);

        auto i = std::find(archives.begin(), archives.end(), archive);

        if (i == archives.end())
        {
            archive->fileSystem = this;
            archives.push_back(archive);
        }
    }

    void FileSystem::removeArchive(Archive* archive)
    {
        auto i = std::find(archives.begin(), archives.end(), archive);

        if (i != archives.end())
        {
            archive->fileSystem = nullptr;
            archives.erase(i);
        }
    }

    std::string FileSystem::getExtensionPart(const std::string& path)
    {
        size_t pos = path.find_last_of('.');

        if (pos != std::string::npos)
        {
            return path.substr(pos + 1);
        }

        return std::string();
    }

    std::string FileSystem::getFilenamePart(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");

        if (pos != std::string::npos)
        {
            return path.substr(pos + 1);
        }
        else
        {
            return path;
        }
    }

    std::string FileSystem::getDirectoryPart(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");

        if (pos != std::string::npos)
        {
            return path.substr(0, pos);
        }

        return std::string();
    }

    bool FileSystem::isAbsolutePath(const std::string& path) const
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        if (MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH) == 0)
        {
            Log(Log::Level::ERR) << "Failed to convert UTF-8 to wide char";
            return false;
        }
        return PathIsRelativeW(szBuffer) == FALSE;
#else
        return !path.empty() && path[0] == '/';
#endif
    }
}
