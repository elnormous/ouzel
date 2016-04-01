// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Language.h"

namespace ouzel
{
    std::string Language::getString(const std::string& str)
    {
        auto i = _strings.find(str);

        if (i != _strings.end())
        {
            return i->second;
        }
        else
        {
            return str;
        }
    }
}
