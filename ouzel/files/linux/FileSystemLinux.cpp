// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include "FileSystemLinux.hpp"
#include "utils/Log.hpp"

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
            Log(Log::Level::INFO) << "Application directory: " << appPath;
        }
        else
        {
            Log(Log::Level::ERR) << "Failed to get current directory";
        }
    }

    std::string FileSystemLinux::getStorageDirectory(bool user) const
    {
        std::string path;

        char* homeDirectory = getenv("XDG_DATA_HOME");

        if (homeDirectory)
        {
            path = homeDirectory;
        }
        else
        {
            passwd* pw = getpwuid(getuid());
            if (!pw)
            {
                Log(Log::Level::ERR) << "Failed to get home directory";
                return "";
            }

            path = pw->pw_dir;
        }

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
