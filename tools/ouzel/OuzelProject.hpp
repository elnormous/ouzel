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
        OuzelProject(const ouzel::storage::Path& path)
        {
            std::string directory = path.getDirectoryPart();

            std::ifstream f(path, std::ios::binary);
            std::vector<std::uint8_t> data{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            json::Data j(data);
            name = j["name"].as<std::string>();

            std::string assetsPath = directory + storage::Path::directorySeparator + j["assetsPath"].as<std::string>();

            for (const auto& assets : j["assets"])
            {

            }
        }

        const std::string& getName() const noexcept { return name; }

    private:
        std::string name;
    };
}

#endif // OUZELPROJECT_HPP
