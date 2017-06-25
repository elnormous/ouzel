// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ouzel
{
    class INI
    {
    public:
        INI();
        INI(const std::string& filename);
        INI(const std::vector<uint8_t>& data);

        bool init(const std::string& filename);
        bool init(const std::vector<uint8_t>& data);

        std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
        void setValue(const std::string& section, const std::string& key, const std::string& value);
        bool deleteValue(const std::string& section, const std::string& key);

    private:
        std::map<std::string, std::map<std::string, std::string>> sections;
    };
}
