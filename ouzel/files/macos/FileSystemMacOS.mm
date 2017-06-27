// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

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

    std::string FileSystemMacOS::getStorageDirectory(bool user) const
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, user ? NSUserDomainMask : NSLocalDomainMask, YES);

        if ([paths count] == 0)
        {
            Log(Log::Level::ERR) << "Failed to get application support directory";
            return "";
        }

        NSString* applicationSupportDirectory = [paths firstObject];

        return [applicationSupportDirectory UTF8String];
    }

    std::string FileSystemMacOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
