// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "FileSystemAndroid.hpp"
#include "core/android/EngineAndroid.hpp"
#include "utils/Log.hpp"

static char TEMP_BUFFER[1024];

namespace ouzel
{
    FileSystemAndroid::FileSystemAndroid()
    {
    }

    std::string FileSystemAndroid::getStorageDirectory(bool) const
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        return engineAndroid->getFilesDirectory();
    }

    bool FileSystemAndroid::readFile(const std::string& filename, std::vector<uint8_t>& data, bool searchResources) const
    {
        if (!isAbsolutePath(filename))
        {
            EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

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

    bool FileSystemAndroid::directoryExists(const std::string& dirname) const
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        AAssetDir* assetDir = AAssetManager_openDir(engineAndroid->getAssetManager(), dirname.c_str());
        bool exists = AAssetDir_getNextFileName(assetDir) != nullptr;
        AAssetDir_close(assetDir);

        if (exists) return true;

        return FileSystem::directoryExists(dirname);
    }

    bool FileSystemAndroid::fileExists(const std::string& filename) const
    {
        EngineAndroid* engineAndroid = static_cast<EngineAndroid*>(engine);

        AAsset* asset = AAssetManager_open(engineAndroid->getAssetManager(), filename.c_str(), AASSET_MODE_STREAMING);

        if (asset)
        {
            AAsset_close(asset);
            return true;
        }

        return FileSystem::fileExists(filename);
    }
}
