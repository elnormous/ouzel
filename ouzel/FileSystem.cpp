// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <sys/stat.h>
#include "CompileConfig.h"
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
    
    bool FileSystem::fileExists(const std::string& filename) const
    {
        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1)
        {
            return true;
        }
        return false;
    }
    
    std::string FileSystem::getPath(const std::string& filename) const
    {
		std::string appPath;

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef urlString = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
        
        CFStringGetCString(urlString, TEMP_BUFFER, sizeof(TEMP_BUFFER), kCFStringEncodingUTF8);
        
        CFRelease(appUrlRef);
        CFRelease(urlString);
#endif

#if defined(OUZEL_PLATFORM_OSX)
		appPath = std::string(TEMP_BUFFER) + "/Contents/Resources/";
#endif
        
#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        appPath = std::string(TEMP_BUFFER);
#endif

#if defined(OUZEL_PLATFORM_WINDOWS)
        wchar_t szBuffer[256];
        GetCurrentDirectoryW(256, szBuffer);

        WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, TEMP_BUFFER, sizeof(TEMP_BUFFER), nullptr, nullptr);

        appPath = std::string(TEMP_BUFFER) + DIRECTORY_SEPARATOR;
#endif

        std::string str = appPath + filename;
        
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
