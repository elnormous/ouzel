// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <pwd.h>
#include <Foundation/Foundation.h>
#include "FileSystemMacOS.h"
#include "utils/Log.h"

namespace ouzel
{
    FileSystemMacOS::FileSystemMacOS()
    {
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* path = [bundle resourcePath];

        appPath = [path UTF8String];
    }

    std::string FileSystemMacOS::getHomeDirectory() const
    {
        passwd* pw = getpwuid(getuid());
        if (!pw)
        {
            Log(Log::Level::ERR) << "Failed to get home directory";
            return "";
        }

        return pw->pw_dir;
    }

    std::string FileSystemMacOS::getStorageDirectory(bool user) const
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, user ? NSUserDomainMask : NSLocalDomainMask, YES);
        NSString* applicationSupportDirectory = [paths firstObject];

        return [applicationSupportDirectory UTF8String];
    }

    std::string FileSystemMacOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
