// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"
#include "FileSystem.h"

#ifdef OUZEL_PLATFORM_OSX
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace ouzel
{
    FileSystem::FileSystem()
    {
        
    }
    
    FileSystem::~FileSystem()
    {
        
    }
    
    std::string FileSystem::getResourcePath(const std::string& filename)
    {
        CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef urlString = CFURLCopyPath(appUrlRef);
        
        char temporaryCString[1024] = "";
        
        CFStringGetCString(urlString, temporaryCString, sizeof(temporaryCString), kCFStringEncodingUTF8);
        
        CFRelease(appUrlRef);
        CFRelease(urlString);
        
        return std::string(temporaryCString) + "Contents/Resources/" + filename;
    }
}
