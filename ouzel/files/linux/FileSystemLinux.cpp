// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <unistd.h>
#include <pwd.h>
#include "FileSystemLinux.h"
#include "utils/Log.h"

extern std::string DEVELOPER_NAME;
extern std::string APPLICATION_NAME;
static char TEMP_BUFFER[1024];

namespace ouzel
{
    FileSystemLinux::FileSystemLinux()
    {
        if (readlink("/proc/self/exe", TEMP_BUFFER, sizeof(TEMP_BUFFER)) != -1)
        {
            appPath = getDirectoryPart(TEMP_BUFFER);
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
    }

    std::string FileSystemLinux::getHomeDirectory() const
    {
        passwd* pw = getpwuid(getuid());
        if (!pw)
        {
            Log(Log::Level::ERR) << "Failed to get home directory";
            return "";
        }

        return pw->pw_dir;
    }

    std::string FileSystemLinux::getStorageDirectory(bool user) const
    {
        std::string path;

        passwd* pw = getpwuid(getuid());
        if (!pw)
        {
            Log(Log::Level::ERR) << "Failed to get home directory";
            return "";
        }

        path = pw->pw_dir;

        path += DIRECTORY_SEPARATOR + "." + DEVELOPER_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        path += DIRECTORY_SEPARATOR + APPLICATION_NAME;

        if (!directoryExists(path))
        {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
            {
                Log(Log::Level::ERR) << "Failed to create directory " << path;
                return "";
            }
        }

        return path;
    }

    std::string FileSystemLinux::getTempDirectory() const
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
