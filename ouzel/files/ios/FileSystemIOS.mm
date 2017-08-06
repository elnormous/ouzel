// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Foundation/Foundation.h>
#include "FileSystemIOS.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    FileSystemIOS::FileSystemIOS()
    {
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* path = [bundle resourcePath];

        appPath = [path UTF8String];
    }

    std::string FileSystemIOS::getStorageDirectory(bool user) const
    {
        NSFileManager* fileManager = [NSFileManager defaultManager];

        NSError* error;
        NSURL* documentDirectory = [fileManager URLForDirectory:NSDocumentDirectory inDomain:user ? NSUserDomainMask : NSLocalDomainMask appropriateForURL:nil create:YES error:&error];

        if (!documentDirectory)
        {
            Log(Log::Level::ERR) << "Failed to get document directory";
            return "";
        }

        return [[documentDirectory path] UTF8String];
    }

    std::string FileSystemIOS::getTempDirectory() const
    {
        NSString* temporaryDirectory = NSTemporaryDirectory();
        return [temporaryDirectory UTF8String];
    }
}
