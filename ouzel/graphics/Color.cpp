// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Color.h"

namespace ouzel
{
    namespace graphics
    {
        Color::Color():
            r(0), g(0), b(0), a(0)
        {
        }

        Color::Color(uint32_t color):
            r(static_cast<uint8_t>((color & 0xFF000000) >> 24)),
            g(static_cast<uint8_t>((color & 0x00FF0000) >> 16)),
            b(static_cast<uint8_t>((color & 0x0000FF00) >> 8)),
            a(static_cast<uint8_t>((color & 0x000000FF)))
        {
        }

        Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha):
            r(red), g(green), b(blue), a(alpha)
        {
        }
    } // namespace graphics
} // namespace ouzel
