// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ouzel
{
    class Archive
    {
    public:
        Archive() {}

        bool readFile(const std::string& filename, std::vector<uint8_t>& data) const;
        bool writeFile(const std::string& filename, const std::vector<uint8_t>& data) const;

        bool fileExists(const std::string& filename) const;
    };
}
