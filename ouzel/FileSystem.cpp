// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <sys/stat.h>
#include "CompileConfig.h"
#if defined(OUZEL_PLATFORM_OSX)
#include <sys/types.h>
#include <pwd.h>
#include <CoreServices/CoreServices.h>
#elif defined(OUZEL_PLATFORM_WINDOWS)
#include <Shlobj.h>
#endif
#include "FileSystem.h"
#include "Utils.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include <CoreFoundation/CoreFoundation.h>
#endif

#if defined(OUZEL_PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace ouzel
{
#ifdef OUZEL_PLATFORM_WINDOWS
	const std::string FileSystem::DIRECTORY_SEPARATOR = "\\";
#else
	const std::string FileSystem::DIRECTORY_SEPARATOR = "/";
#endif
    
	FileSystem::FileSystem()
    {
        
    }
    
    FileSystem::~FileSystem()
    {
        
    }
    
    std::string FileSystem::getHomeDirectory()
    {
#if defined(OUZEL_PLATFORM_OSX)
        struct passwd* pw = getpwuid(getuid());
        if (pw)
        {
            return pw->pw_dir;
        }
#elif defined(OUZEL_PLATFORM_WINDOWS)
        WCHAR szBuffer[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, szBuffer)))
        {
            WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
            return TEMP_BUFFER;
        }
#endif
        return "";
    }
    
    std::string FileSystem::getStorageDirectory(const std::string& developer, const std::string& app)
    {
        std::string path;
        
#if defined(OUZEL_PLATFORM_OSX)
        FSRef ref;
        OSType folderType = kApplicationSupportFolderType;
        
        FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );
        
        FSRefMakePath( &ref, (UInt8*)&TEMP_BUFFER, sizeof(TEMP_BUFFER));
        
        path = TEMP_BUFFER;
        
        CFStringRef bundleIdentifier = CFBundleGetIdentifier(CFBundleGetMainBundle());
        CFStringGetCString(bundleIdentifier, TEMP_BUFFER, sizeof(TEMP_BUFFER), kCFStringEncodingUTF8);
        CFRelease(bundleIdentifier);
        
        path += DIRECTORY_SEPARATOR + TEMP_BUFFER;
        
        if (!directoryExists(path))
        {
            mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
#elif  defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        //TODO: implement
#elif defined(OUZEL_PLATFORM_WINDOWS)
        WCHAR szBuffer[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szBuffer)))
        {
            WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);
            path = TEMP_BUFFER;
        }
        
        path += DIRECTORY_SEPARATOR + developer;
        
        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            CreateDirectory(szBuffer, NULL);
        }
        
        path += DIRECTORY_SEPARATOR + app;
        
        if (!directoryExists(path))
        {
            MultiByteToWideChar(CP_ACP, 0, path.c_str(), -1, szBuffer, MAX_PATH);
            CreateDirectory(szBuffer, NULL);
        }
#endif
        return path;
    }
    
    bool FileSystem::directoryExists(const std::string& filename) const
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1)
        {
            return true;
        }
        
        return (buf.st_mode & S_IFMT) == S_IFDIR;
    }
    
    bool FileSystem::fileExists(const std::string& filename) const
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1)
        {
            return true;
        }
        
        return (buf.st_mode & S_IFMT) == S_IFREG;
    }
    
    std::string FileSystem::getPath(const std::string& filename) const
    {
		std::string appPath;

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        CFURLRef resourcesUrlRef = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
        CFURLRef absoluteURL = CFURLCopyAbsoluteURL(resourcesUrlRef);
        
        CFStringRef urlString = CFURLCopyFileSystemPath(absoluteURL, kCFURLPOSIXPathStyle);
        
        CFStringGetCString(urlString, TEMP_BUFFER, sizeof(TEMP_BUFFER), kCFStringEncodingUTF8);
        
        CFRelease(resourcesUrlRef);
        CFRelease(absoluteURL);
        CFRelease(urlString);
        
        appPath = std::string(TEMP_BUFFER);
#endif

#if defined(OUZEL_PLATFORM_WINDOWS)
        wchar_t szBuffer[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, szBuffer);

        WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);

        appPath = std::string(TEMP_BUFFER);
#endif

        std::string str = appPath + DIRECTORY_SEPARATOR + filename;
        
        if (fileExists(str))
        {
            return str;
        }
        else
        {
            for (const std::string& path : _resourcePaths)
            {
                str = appPath + DIRECTORY_SEPARATOR + path + DIRECTORY_SEPARATOR + filename;
                
                if (fileExists(str))
                {
                    return str;
                }
            }
        }
        
        return "";
    }
    
    void FileSystem::addResourcePath(const std::string& path)
    {
        std::vector<std::string>::iterator i = std::find(_resourcePaths.begin(), _resourcePaths.end(), path);
        
        if (i == _resourcePaths.end())
        {
            _resourcePaths.push_back(path);
        }
    }
    
    std::string FileSystem::getExtension(const std::string& path) const
    {
        std::string result;
        
        size_t pos = path.find_last_of('.');
        
        if (pos != std::string::npos)
        {
            result = path.substr(pos + 1);
        }
        
        return result;
    }
}
