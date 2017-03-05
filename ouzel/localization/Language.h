// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <unordered_map>
#include <string>

namespace ouzel
{
    class Language
    {
    public:
        bool initFromFile(const std::string& filename);

        std::string getString(const std::string& str);

    protected:
        std::unordered_map<std::string, std::string> strings;
    };
}
