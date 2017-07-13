// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Foundation/Foundation.h>
#include "FileSystemMacOS.h"
#include "utils/Log.h"

extern std::string DEVELOPER_NAME;
extern std::string APPLICATION_NAME;

namespace ouzel
{
    FileSystemMacOS::FileSystemMacOS()
    {
        fileManager = [NSFileManager defaultManager];

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
        NSString* path = [NSString stringWithFormat:@"%@/%s/%s", applicationSupportDirectory, DEVELOPER_NAME.c_str(), APPLICATION_NAME.c_str()];

        if (![fileManager fileExistsAtPath:path isDirectory:Nil])
        {
            [fileManager createDirectoryAtPath:path withIntermediateDirectories:YES attributes:Nil error:Nil];
        }

        return [path UTF8String];
    }

    std::string FileSystemMacOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
