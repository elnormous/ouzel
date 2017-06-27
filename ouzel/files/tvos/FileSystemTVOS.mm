// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <unistd.h>
#include <pwd.h>
#include <Foundation/Foundation.h>
#include "FileSystemTVOS.h"

static char TEMP_BUFFER[1024];

namespace ouzel
{
    FileSystemTVOS::FileSystemTVOS()
    {
        NSBundle* bundle = [NSBundle mainBundle];
        NSString* path = [bundle resourcePath];

        appPath = [path UTF8String];
    }

    std::string FileSystemTVOS::getHomeDirectory() const
    {
        NSString* path = NSTemporaryDirectory();
        return [path UTF8String];
    }

    std::string FileSystemTVOS::getStorageDirectory(bool user) const
    {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, user ? NSUserDomainMask : NSLocalDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];

        return [documentsDirectory UTF8String];
    }

    std::string FileSystemTVOS::getTempDirectory() const
    {
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
    }
}
