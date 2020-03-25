// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_OUZELPROJECT_HPP
#define OUZEL_OUZELPROJECT_HPP

#include <fstream>
#include "storage/FileSystem.hpp"
#include "utils/Json.hpp"
#include "Platform.hpp"

namespace ouzel
{
    class OuzelProject final
    {
    public:
        OuzelProject(const storage::Path& path)
        {
            directoryPath = path.getDirectory();

            std::ifstream f(path, std::ios::binary);
            std::vector<char> data{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            json::Data j(data);
            name = j["name"].as<std::string>();
            identifier = j["identifier"].as<std::string>();

            ouzelPath = directoryPath / j["ouzelPath"].as<std::string>();

            for (const auto& platform : j["platforms"])
                if (platform.as<std::string>() == "windows")
                    platforms.insert(Platform::Windows);
                else if (platform.as<std::string>() == "macos")
                    platforms.insert(Platform::MacOS);
                else if (platform.as<std::string>() == "linux")
                    platforms.insert(Platform::Linux);
                else if (platform.as<std::string>() == "ios")
                    platforms.insert(Platform::Ios);
                else if (platform.as<std::string>() == "tvos")
                    platforms.insert(Platform::Tvos);
                else if (platform.as<std::string>() == "android")
                    platforms.insert(Platform::Android);
                else if (platform.as<std::string>() == "emscripten")
                    platforms.insert(Platform::Emscripten);
                else
                    throw std::runtime_error("Invalid platform");

            sourcePath = directoryPath / j["sourcePath"].as<std::string>();
            assetsPath = directoryPath / j["assetsPath"].as<std::string>();
        }

        const storage::Path& getDirectoryPath() const noexcept { return directoryPath; }
        const std::string& getName() const noexcept { return name; }
        const std::string& getIdentifier() const noexcept { return identifier; }
        const storage::Path& getOuzelPath() const noexcept { return ouzelPath; }
        const std::set<Platform>& getPlatforms() const noexcept { return platforms; }
        const storage::Path& getSourcePath() const noexcept { return sourcePath; }
        const storage::Path& getAssetsPath() const noexcept { return assetsPath; }

        void exportAssets()
        {
            /*storage::Path resourcesPath = directoryPath / storage::Path("Resources");

            for (const auto& asset : j["assets"])
            {
                storage::Path assetPath(assetsPath / asset["path"].as<std::string>());

                storage::Path resourceName = asset["path"].as<std::string>();
                storage::Path resourcePath = resourcesPath / resourceName.getStem() + std::string(".otexture");

                // TODO: check if input file exists
                // TODO: check if output file exists and is older than the input file
                // TODO: export input file to output file
            }*/
        }

    private:
        storage::Path directoryPath;
        std::string name;
        std::string identifier;
        storage::Path ouzelPath;
        std::set<Platform> platforms;
        storage::Path sourcePath;
        storage::Path assetsPath;
    };
}

#endif // OUZELPROJECT_HPP
