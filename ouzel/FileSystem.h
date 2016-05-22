// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#if defined(OUZEL_PLATFORM_ANDROID)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif
#include "Noncopyable.h"

namespace ouzel
{
#if defined(OUZEL_PLATFORM_ANDROID)
    extern AAssetManager* assetManager;
#endif

    class Engine;

    class FileSystem: public Noncopyable
    {
        friend Engine;
    public:
        static const std::string DIRECTORY_SEPARATOR;

        virtual ~FileSystem();

        std::string getHomeDirectory();
        std::string getStorageDirectory(const std::string& developer, const std::string& app);

        bool loadFile(const std::string& filename, std::vector<uint8_t>& data) const;

        std::string getPath(const std::string& filename) const;
        void addResourcePath(const std::string& path);

        std::string getExtensionPart(const std::string& path) const;
        std::string getFilenamePart(const std::string& path) const;
        std::string getDirectoryPart(const std::string& path) const;
        bool isAbsolutePath(const std::string& path) const;

        bool directoryExists(const std::string& filename) const;
        bool fileExists(const std::string& filename) const;

    protected:
        FileSystem();

        std::vector<std::string> resourcePaths;
    };
}
