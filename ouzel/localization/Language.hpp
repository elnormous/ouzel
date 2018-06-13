// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    class Language final
    {
    public:
        bool init(const std::string& filename);
        bool init(const std::vector<uint8_t>& data);

        std::string getString(const std::string& str);

    private:
        std::map<std::string, std::string> strings;
    };
}
