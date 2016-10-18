// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <stdint.h>

namespace ouzel
{
    namespace graphics
    {
        class Color
        {
        public:
            enum: uint32_t
            {
                BLACK = 0x000000ff,
                RED = 0xff0000ff,
                MAGENTA = 0xff00ffff,
                GREEN = 0x00ff00ff,
                CYAN = 0x00ffffff,
                BLUE = 0x0000ffff,
                YELLOW = 0xffff00ff,
                WHITE = 0xffffffff
            };

            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;

            Color();
            Color(uint32_t color);
            Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);

            float getR() const { return r / 255.0f; }
            float getG() const { return g / 255.0f; }
            float getB() const { return b / 255.0f; }
            float getA() const { return a / 255.0f; }

            uint32_t getIntValue() const
            {
                return ((static_cast<uint32_t>(r) << 24) |
                        (static_cast<uint32_t>(g) << 16) |
                        (static_cast<uint32_t>(b) << 8) |
                        static_cast<uint32_t>(a));
            }
        };
    } // namespace graphics
} // namespace ouzel
