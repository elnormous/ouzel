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

        uint8_t v[4];

        Color()
        {
            v[0] = 0;
            v[1] = 0;
            v[2] = 0;
            v[3] = 0;
        }

        Color(uint32_t color)
        {
            v[0] = static_cast<uint8_t>((color & 0xFF000000) >> 24);
            v[1] = static_cast<uint8_t>((color & 0x00FF0000) >> 16);
            v[2] = static_cast<uint8_t>((color & 0x0000FF00) >> 8);
            v[3] = static_cast<uint8_t>(color & 0x000000FF);
        }

        Color& operator=(uint32_t color)
        {
            v[0] = static_cast<uint8_t>((color & 0xFF000000) >> 24);
            v[1] = static_cast<uint8_t>((color & 0x00FF0000) >> 16);
            v[2] = static_cast<uint8_t>((color & 0x0000FF00) >> 8);
            v[3] = static_cast<uint8_t>(color & 0x000000FF);

            return *this;
        }

        Color(const std::string& color);
        Color& operator=(const std::string& color);

        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF)
        {
            v[0] = red;
            v[1] = green;
            v[2] = blue;
            v[3] = alpha;
        }

        Color(const Vector3& vec);
        Color& operator=(const Vector3& vec);

        Color(const Vector4& vec);
        Color& operator=(const Vector4& vec);

        uint8_t& r() { return v[0]; }
        uint8_t& g() { return v[1]; }
        uint8_t& b() { return v[2]; }
        uint8_t& a() { return v[3]; }
        uint8_t r() const { return v[0]; }
        uint8_t g() const { return v[1]; }
        uint8_t b() const { return v[2]; }
        uint8_t a() const { return v[3]; }
        uint8_t& operator[](size_t index) { return v[index]; }
        uint8_t operator[](size_t index) const { return v[index]; }

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
