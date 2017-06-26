// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <unistd.h>
#include <pwd.h>
#include <Foundation/Foundation.h>
#include "FileSystemMacOS.h"
#include "utils/Log.h"

static char TEMP_BUFFER[1024];

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
