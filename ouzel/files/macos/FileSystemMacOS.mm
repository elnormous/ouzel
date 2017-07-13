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
        NSFileManager* fileManager = [NSFileManager defaultManager];

        NSError* error;
        NSURL* applicationSupportDirectory = [fileManager URLForDirectory:NSApplicationSupportDirectory inDomain:user ? NSUserDomainMask : NSLocalDomainMask appropriateForURL:nil create:YES error:&error];

        if (!applicationSupportDirectory)
        {
            Log(Log::Level::ERR) << "Failed to get application support directory";
            return "";
        }

        NSString* identifier = [[NSBundle mainBundle] bundleIdentifier];

        NSURL* path = [applicationSupportDirectory URLByAppendingPathComponent:identifier];

        [fileManager createDirectoryAtURL:path withIntermediateDirectories:YES attributes:Nil error:Nil];

        return [[path path] UTF8String];
    }

    std::string FileSystemMacOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
