// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Font.hpp"

namespace ouzel
{
    class BMFont: public Font
    {
    public:
        BMFont();
        BMFont(const std::string& filename, bool mipmaps = true);

    protected:
        bool parseFont(const std::string& filename, bool mipmaps = true);
    };
}
