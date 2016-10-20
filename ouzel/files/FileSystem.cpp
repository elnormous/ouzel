// Copyright (C) 2016 Elviss Strazdins
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
    #define NOMINMAX
    #include <windows.h>
    #include <Shlobj.h>
    #include <Shlwapi.h>
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
    #include <unistd.h>
    #include <pwd.h>
#endif
#include "FileSystem.h"
#include "utils/Log.h"
#include "utils/Utils.h"

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace ouzel
{
    static char TEMP_BUFFER[1024];

#if OUZEL_PLATFORM_WINDOWS
    const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
    const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif

#if OUZEL_PLATFORM_ANDROID
    AAssetManager* assetManager = nullptr;
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
        char* exePath;

        if (_get_pgmptr(&exePath) == 0)
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
        struct passwd* pw = getpwuid(getuid());
        if (pw)
        {
            return pw->pw_dir;
        }
#elif OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, szBuffer)))
        {
            WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
            return TEMP_BUFFER;
        }
#endif
        return "";
    }

    std::string FileSystem::getStorageDirectory(const std::string& developer, const std::string& app)
    {
        std::string path;

#if OUZEL_PLATFORM_MACOS
        OUZEL_UNUSED(developer);
        OUZEL_UNUSED(app);

        FSRef ref;
        OSType folderType = kApplicationSupportFolderType;

        FSFindFolder(kUserDomain, folderType, kCreateFolder, &ref);

        FSRefMakePath(&ref, reinterpret_cast<UInt8*>(&TEMP_BUFFER), sizeof(TEMP_BUFFER));

        path = TEMP_BUFFER;

        CFStringRef bundleIdentifier = CFBundleGetIdentifier(CFBundleGetMainBundle());
        CFStringGetCString(bundleIdentifier, TEMP_BUFFER, sizeof(TEMP_BUFFER), kCFStringEncodingUTF8);

        path += DIRECTORY_SEPARATOR + TEMP_BUFFER;

        if (!directoryExists(path))
        {
            mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        OUZEL_UNUSED(developer);
        OUZEL_UNUSED(app);
        //TODO: implement
#elif OUZEL_PLATFORM_WINDOWS
        WCHAR szBuffer[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szBuffer)))
        {
            WideCharToMultiByte(CP_UTF8, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
            path = TEMP_BUFFER;
        }

        path += DIRECTORY_SEPARATOR + developer;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            CreateDirectory(szBuffer, NULL);
        }

        path += DIRECTORY_SEPARATOR + app;

        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            CreateDirectory(szBuffer, NULL);
        }
#elif OUZEL_PLATFORM_ANDROID
        OUZEL_UNUSED(developer);
        OUZEL_UNUSED(app);
        //TODO: implement
#elif OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_EMSCRIPTEN
        OUZEL_UNUSED(developer);
        OUZEL_UNUSED(app);
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

    bool FileSystem::loadFile(const std::string& filename, std::vector<uint8_t>& data) const
    {
#if OUZEL_PLATFORM_ANDROID
        if (!isAbsolutePath(filename))
        {
            AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_STREAMING);

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
        std::string path = getPath(filename);

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
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        return true;
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

    std::string FileSystem::getPath(const std::string& filename) const
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
            else
            {
                for (const std::string& path : resourcePaths)
                {
                    str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;

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
        std::vector<std::string>::iterator i = std::find(resourcePaths.begin(), resourcePaths.end(), path);

        if (i == resourcePaths.end())
        {
            resourcePaths.push_back(path);
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
