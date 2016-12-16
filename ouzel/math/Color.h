// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <stdint.h>

namespace ouzel
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

        uint8_t v[4];

        Color();
        Color(uint32_t color);
        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);

        float normR() const { return v[0] / 255.0f; }
        float normG() const { return v[1] / 255.0f; }
        float normB() const { return v[2] / 255.0f; }
        float normA() const { return v[3] / 255.0f; }

        uint32_t getIntValue() const
        {
            return (static_cast<uint32_t>(v[0]) << 24) |
                   (static_cast<uint32_t>(v[1]) << 16) |
                   (static_cast<uint32_t>(v[2]) << 8) |
                   static_cast<uint32_t>(v[3]);
        }
    };
} // namespace ouzel
