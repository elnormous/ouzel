// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
            storage::Path directory = path.getDirectory();

            std::ifstream f(path, std::ios::binary);
            std::vector<char> data{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            json::Data j(data);
            name = j["name"].as<std::string>();

            storage::Path assetsPath = directory + "/" + j["assetsPath"].as<std::string>();
            storage::Path resourcesPath = directory + "/Resources/";

            for (const auto& asset : j["assets"])
            {
                storage::Path assetPath(assetsPath + asset["path"].as<std::string>());

                storage::Path resourceName = asset["path"].as<std::string>();
                storage::Path resourcePath = resourcesPath + resourceName.getStem() + ".otexture";

                // TODO: check if input file exists
                // TODO: check if output file exists and is older than the input file
                // TODO: export input file to output file
            }
        }

        const std::string& getName() const noexcept { return name; }

    private:
        std::string name;
    };
}

#endif // OUZELPROJECT_HPP
