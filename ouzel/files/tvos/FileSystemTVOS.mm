// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Foundation/Foundation.h>
#include "FileSystemTVOS.h"
#include "utils/Log.h"

namespace ouzel
{
    FileSystemTVOS::FileSystemTVOS()
    {
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* path = [bundle resourcePath];

        appPath = [path UTF8String];
    }

    std::string FileSystemTVOS::getStorageDirectory(bool user) const
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, user ? NSUserDomainMask : NSLocalDomainMask, YES);

        if ([paths count] == 0)
        {
            Log(Log::Level::ERR) << "Failed to get application support directory";
            return "";
        }

        NSString* documentsDirectory = [paths objectAtIndex:0];

        return [documentsDirectory UTF8String];
    }

    std::string FileSystemTVOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
