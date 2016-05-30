// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>
#include "utils/Types.h"

namespace ouzel
{
    class Language
    {
    public:
        static LanguagePtr createFromFile(const std::string& filename);

        bool initFromFile(const std::string& filename);

        std::string getString(const std::string& str);

    protected:
        std::map<std::string, std::string> strings;
    };
}
