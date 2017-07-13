// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "FileSystemAndroid.h"
#include "core/android/EngineAndroid.h"
#include "utils/Log.h"

static char TEMP_BUFFER[1024];

namespace ouzel
{
    FileSystemAndroid::FileSystemAndroid()
    {
    }

    std::string FileSystemAndroid::getStorageDirectory(bool) const
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(sharedEngine);

        return engineAndroid->getFilesDirectory();
    }

    std::string FileSystemAndroid::getTempDirectory() const
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(sharedEngine);

        return engineAndroid->getCacheDirectory();
    }

    bool FileSystemAndroid::readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources) const
    {
        if (!isAbsolutePath(filename))
        {
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(sharedEngine);

            AAsset* asset = AAssetManager_open(engineAndroid->getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

            if (!asset)
            {
                Log(Log::Level::ERR) << "Failed to open file " << filename;
                return false;
            }

            int bytesRead = 0;

            while ((bytesRead = AAsset_read(asset, TEMP_BUFFER, sizeof(TEMP_BUFFER))) > 0)
            {
                data.insert(data.end(), reinterpret_cast<uint8_t*>(TEMP_BUFFER), reinterpret_cast<uint8_t*>(TEMP_BUFFER + bytesRead));
            }

            AAsset_close(asset);

            return true;
        }
        else
        {
            return FileSystem::readFile(filename, data, searchResources);
        }
    }
}
