// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "gui/BMFont.hpp"

namespace ouzel
{
    enum UTFChars
    {
        ASCII = 1,
        ASCIIPLUS = 1 << 1

    };
    class TTFont : public BMFont
    {
    public:
        TTFont();
        TTFont(const std::string& filename, uint16_t pt, UTFChars flag = ASCII);

    protected:
        bool parseFont(const std::string& filename, uint16_t pt, UTFChars flag = ASCII);
    };
}
