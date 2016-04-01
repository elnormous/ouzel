// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>

namespace ouzel
{
    class Language
    {
    public:
        std::string getString(const std::string& str);

    private:
        std::map<std::string, std::string> _strings;
    };
}
