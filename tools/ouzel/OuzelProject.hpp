// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_OUZELPROJECT_HPP
#define OUZEL_OUZELPROJECT_HPP

#include <fstream>
#include "storage/FileSystem.hpp"
#include "utils/Json.hpp"

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

            assetsPath = directoryPath / j["assetsPath"].as<std::string>();
            ouzelPath = directoryPath / j["ouzelPath"].as<std::string>();
        }

        const storage::Path& getDirectoryPath() const noexcept { return directoryPath; }
        const std::string& getName() const noexcept { return name; }
        const std::string& getIdentifier() const noexcept { return identifier; }
        const storage::Path& getOuzelPath() const noexcept { return ouzelPath; }
        const storage::Path& getAssetsPath() const noexcept { return assetsPath; }

        void exportAssets()
        {
        }

    private:
        storage::Path directoryPath;
        std::string name;
        std::string identifier;
        storage::Path ouzelPath;
        storage::Path assetsPath;
    };
}

#endif // OUZELPROJECT_HPP
