// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include "core/CompileConfig.h"
#if OUZEL_PLATFORM_MACOS
    #include <sys/types.h>
    #include <pwd.h>
    #include <CoreServices/CoreServices.h>
#elif OUZEL_PLATFORM_WINDOWS
    #include <windows.h>
    #include <Shlobj.h>
    #include <Shlwapi.h>
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #include <unistd.h>
    #include <pwd.h>
#endif
#include "FileSystem.h"
#include "Archive.h"
#include "utils/Log.h"

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <CoreFoundation/CoreFoundation.h>
#elif OUZEL_PLATFORM_ANDROID
#include "core/android/ApplicationAndroid.h"
#endif

extern std::string DEVELOPER_NAME;
extern std::string APPLICATION_NAME;

namespace ouzel
{
    static char TEMP_BUFFER[1024];

#if OUZEL_PLATFORM_WINDOWS
    const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
    const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif

    FileSystem::FileSystem()
    {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        CFURLRef resourcesUrlRef = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
        CFURLRef absoluteURL = CFURLCopyAbsoluteURL(resourcesUrlRef);

        CFStringRef urlString = CFURLCopyFileSystemPath(absoluteURL, kCFURLPOSIXPathStyle);

        CFStringGetCString(urlString, TEMP_BUFFER, sizeof(TEMP_BUFFER), kCFStringEncodingUTF8);

        CFRelease(resourcesUrlRef);
        CFRelease(absoluteURL);
        CFRelease(urlString);

        appPath = std::string(TEMP_BUFFER);
#elif OUZEL_PLATFORM_WINDOWS
        char* exePath = _pgmptr;

        if (exePath)
        {
            appPath = getDirectoryPart(exePath);
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        if (readlink("/proc/self/exe", TEMP_BUFFER, sizeof(TEMP_BUFFER)) != -1)
        {
            appPath = getDirectoryPart(TEMP_BUFFER);
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
#endif
    }

    std::string FileSystem::getHomeDirectory()
    {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        passwd* pw = getpwuid(getuid());
        if (!pw)
        {
            Log(Log::Level::ERR) << "Failed to get home directory";
            return "";
        }

        return pw->pw_dir;
#elif OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        HRESULT hr = SHGetFolderPathW(nullptr, CSIDL_PROFILE, nullptr, SHGFP_TYPE_CURRENT, szBuffer);
        if (FAILED(hr))
        {
            Log(Log::Level::ERR) << "Failed to get the path of the profile directory, error: " << hr;
            return "";
        }

        WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
        return TEMP_BUFFER;
#endif
        return "";
    }

    std::string FileSystem::getStorageDirectory(bool user)
    {
        std::string path;

#if OUZEL_PLATFORM_MACOS
        FSRef ref;

        OSErr err = FSFindFolder(user ? kUserDomain : kLocalDomain, kApplicationSupportFolderType, kCreateFolder, &ref);

        if (err != noErr)
        {
            Log(Log::Level::ERR) << "Failed to get the path of the Application Support directory, error: " << err;
            return "";
        }

        err = FSRefMakePath(&ref, reinterpret_cast<UInt8*>(&TEMP_BUFFER), sizeof(TEMP_BUFFER));

        if (err != noErr)
        {
            Log(Log::Level::ERR) << "Failed to make path from FSRef, error: " << err;
            return "";
        }

        path = TEMP_BUFFER;

        path += DIRECTORY_SEPARATOR + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        OUZEL_UNUSED(DEVELOPER_NAME);
        OUZEL_UNUSED(APPLICATION_NAME);
        //TODO: implement
#elif OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        HRESULT hr = SHGetFolderPathW(nullptr, (user ? CSIDL_LOCAL_APPDATA : CSIDL_COMMON_APPDATA) | CSIDL_FLAG_CREATE, nullptr, SHGFP_TYPE_CURRENT, szBuffer);
        if (FAILED(hr))
        {
            Log(Log::Level::ERR) << "Failed to get the path of the AppData directory, error: " << hr;
            return "";
        }

        WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
        path = TEMP_BUFFER;

        path += DIRECTORY_SEPARATOR + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            if (!CreateDirectoryW(szBuffer, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            if (!CreateDirectoryW(szBuffer, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }
#elif OUZEL_PLATFORM_ANDROID
        OUZEL_UNUSED(DEVELOPER_NAME);
        OUZEL_UNUSED(APPLICATION_NAME);
        //TODO: implement
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        passwd* pw = getpwuid(getuid());
        if (!pw)
        {
            Log(Log::Level::ERR) << "Failed to get home directory";
            return "";
        }

        path = pw->pw_dir;

        path += DIRECTORY_SEPARATOR + "." + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }
#elif OUZEL_PLATFORM_EMSCRIPTEN
        OUZEL_UNUSED(DEVELOPER_NAME);
        OUZEL_UNUSED(APPLICATION_NAME);
        //TODO: implement
#endif
        return path;
    }

    std::string FileSystem::getTempDirectory()
    {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS

        if (confstr(_CS_DARWIN_USER_TEMP_DIR, TEMP_BUFFER, sizeof(TEMP_BUFFER)))
        {
            return TEMP_BUFFER;
        }
        else
        {
            char const* path = getenv("TMPDIR");
            if (path)
            {
                return path;
            }
            else
            {
                return "/tmp";
            }
        }

#elif OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        if (GetTempPathW(MAX_PATH, szBuffer))
        {
            WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);

            return TEMP_BUFFER;
        }
#elif OUZEL_PLATFORM_ANDROID
        //TODO: implement
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
        char const* path = getenv("TMPDIR");
        if (path)
        {
            return path;
        }
        else
        {
            return "/tmp";
        }
#endif

        return "";
    }

    bool FileSystem::readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources) const
    {
#if OUZEL_PLATFORM_ANDROID
        if (!isAbsolutePath(filename))
        {
            ApplicationAndroid* applicationAndroid = static_cast<ApplicationAndroid*>(sharedApplication);

            AAsset* asset = AAssetManager_open(applicationAndroid->getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

            if (!asset)
            {
                Log(Log::Level::ERR) << "Failed to open file " << filename;
                return false;
            }

            int bytesRead = 0;

            while ((bytesRead = AAsset_read(asset, TEMP_BUFFER, sizeof(TEMP_BUFFER))) > 0)
            {
                data.insert(data.end(), reinterpret_cast<uint8_t*>(TEMP_BUFFER), reinterpret_cast<uint8_t*>(TEMP_BUFFER + bytesRead));
            }

            AAsset_close(asset);

            return true;
        }
#endif

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

    bool FileSystem::directoryExists(const std::string& filename)
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != 0)
        {
            return false;
        }

        return (buf.st_mode & S_IFMT) == S_IFDIR;
    }

    bool FileSystem::fileExists(const std::string& filename)
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
        if (!searchResources || isAbsolutePath(filename))
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

    void FileSystem::addArchive(std::shared_ptr<Archive>& archive)
    {
        auto i = std::find(archives.begin(), archives.end(), archive);

        if (i == archives.end())
        {
            archives.push_back(archive);
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

    bool FileSystem::isAbsolutePath(const std::string& path)
    {
#if OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
        return PathIsRelativeW(szBuffer) == FALSE;
#else
        return !path.empty() && path[0] == '/';
#endif
    }
}
