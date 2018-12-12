// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_COLOR_HPP
#define OUZEL_MATH_COLOR_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace ouzel
{
    class Vector3;
    class Vector4;

    class Color final
    {
    public:
        static constexpr uint32_t BLACK = 0x000000ff;
        static constexpr uint32_t RED = 0xff0000ff;
        static constexpr uint32_t MAGENTA = 0xff00ffff;
        static constexpr uint32_t GREEN = 0x00ff00ff;
        static constexpr uint32_t CYAN = 0x00ffffff;
        static constexpr uint32_t BLUE = 0x0000ffff;
        static constexpr uint32_t YELLOW = 0xffff00ff;
        static constexpr uint32_t WHITE = 0xffffffff;
        static constexpr uint32_t GRAY = 0x808080ff;

        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;

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

        explicit Color(float color[4]):
            r(static_cast<uint8_t>(color[0] * 255.0F)),
            g(static_cast<uint8_t>(color[1] * 255.0F)),
            b(static_cast<uint8_t>(color[2] * 255.0F)),
            a(static_cast<uint8_t>(color[3] * 255.0F))
        {
        }

        explicit Color(const Vector3& vec);
        Color& operator=(const Vector3& vec);

        explicit Color(const Vector4& vec);
        Color& operator=(const Vector4& vec);

        inline float normR() const { return r / 255.0F; }
        inline float normG() const { return g / 255.0F; }
        inline float normB() const { return b / 255.0F; }
        inline float normA() const { return a / 255.0F; }

        inline uint32_t getIntValue() const
        {
            return (static_cast<uint32_t>(r) << 24) |
                   (static_cast<uint32_t>(g) << 16) |
                   (static_cast<uint32_t>(b) << 8) |
                   static_cast<uint32_t>(a);
        }
    };
} // namespace ouzel

#endif // OUZEL_MATH_COLOR_HPP
