// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace ouzel
{
    class Vector3;
    class Vector4;

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
            WHITE = 0xffffffff,
            GRAY = 0x808080ff
        };

        union
        {
            struct
            {
                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0;
                uint8_t a = 0;
            };
            uint8_t v[4];
        };

        Color()
        {
        }

        Color(uint32_t color):
            r(static_cast<uint8_t>((color & 0xFF000000) >> 24)),
            g(static_cast<uint8_t>((color & 0x00FF0000) >> 16)),
            b(static_cast<uint8_t>((color & 0x0000FF00) >> 8)),
            a(static_cast<uint8_t>(color & 0x000000FF))
        {
        }

        Color& operator=(uint32_t color)
        {
            r = static_cast<uint8_t>((color & 0xFF000000) >> 24);
            g = static_cast<uint8_t>((color & 0x00FF0000) >> 16);
            b = static_cast<uint8_t>((color & 0x0000FF00) >> 8);
            a = static_cast<uint8_t>(color & 0x000000FF);

            return *this;
        }

        Color(const std::string& color);
        Color& operator=(const std::string& color);

        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF):
            r(red), g(green), b(blue), a(alpha)
        {
        }

        Color(const Vector3& vec);
        Color& operator=(const Vector3& vec);

        Color(const Vector4& vec);
        Color& operator=(const Vector4& vec);

        float normR() const { return r / 255.0f; }
        float normG() const { return g / 255.0f; }
        float normB() const { return b / 255.0f; }
        float normA() const { return a / 255.0f; }

        uint32_t getIntValue() const
        {
            return (static_cast<uint32_t>(r) << 24) |
                   (static_cast<uint32_t>(g) << 16) |
                   (static_cast<uint32_t>(b) << 8) |
                   static_cast<uint32_t>(a);
        }
    };
} // namespace ouzel
