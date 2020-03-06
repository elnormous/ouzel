// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_OUZELPROJECT_HPP
#define OUZEL_OUZELPROJECT_HPP

#include <fstream>
#include "storage/File.hpp"
#include "storage/FileSystem.hpp"
#include "utils/Json.hpp"

namespace ouzel
{
    class OuzelProject final
    {
    public:
        OuzelProject(const std::string& path)
        {
            std::string directory = storage::FileSystem::getDirectoryPart(path);

            std::vector<uint8_t> data;
            uint8_t buffer[1024];

            storage::File file(path, storage::File::Mode::Read);
            while (const uint32_t size = file.read(buffer, sizeof(buffer)))
                data.insert(data.end(), buffer, buffer + size);

            json::Data j(data);
            name = j["name"].as<std::string>();

            std::string assetsPath = directory + storage::Path::DIRECTORY_SEPARATOR + j["assetsPath"].as<std::string>();

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
