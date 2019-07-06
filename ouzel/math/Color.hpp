// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_COLOR_HPP
#define OUZEL_MATH_COLOR_HPP

#include <cstddef>
#include <cstdint>
#include <array>
#include <string>
#include "Vector.hpp"

namespace ouzel
{
    class Color final
    {
    public:
        uint8_t v[4]{0};

        constexpr Color() noexcept {}

        constexpr Color(uint32_t color) noexcept:
            v{
                static_cast<uint8_t>((color & 0xFF000000) >> 24),
                static_cast<uint8_t>((color & 0x00FF0000) >> 16),
                static_cast<uint8_t>((color & 0x0000FF00) >> 8),
                static_cast<uint8_t>(color & 0x000000FF)
            }
        {
        }

        constexpr uint8_t hexCharToString(char c)
        {
            return (c >= '0' && c <= '9') ? c - '0' :
                (c >= 'a' && c <= 'f') ? c - 'a' + 10 :
                (c >= 'A' && c <= 'F') ? c - 'A' + 10 :
                0;
        }

        Color(const std::string& color)
        {
            if (!color.empty())
            {
                if (color.front() == '#')
                {
                    assert(color.length() == 4 || color.length() == 7);

                    const size_t componentSize = (color.length() - 1) / 3; // exclude the #

                    for (size_t component = 0; component < 3; ++component)
                    {
                        v[component] = 0;

                        for (size_t byte = 0; byte < 2; ++byte)
                        {
                            char c = (byte < componentSize) ? color[component * componentSize + byte + 1] : color[component * componentSize + 1];
                            v[component] = static_cast<uint8_t>((v[component] << 4) | hexCharToString(c));
                        }
                    }

                    v[3] = 0xFF; // alpha
                }
                else
                {
                    auto intValue = static_cast<uint32_t>(std::stoul(color));
                    v[0] = static_cast<uint8_t>((intValue & 0xFF000000) >> 24);
                    v[1] = static_cast<uint8_t>((intValue & 0x00FF0000) >> 16);
                    v[2] = static_cast<uint8_t>((intValue & 0x0000FF00) >> 8);
                    v[3] = static_cast<uint8_t>(intValue & 0x000000FF);
                }
            }
            else
                for (size_t i = 0; i < 4; ++i)
                    v[i] = 0;
        }

        Color(const char* color):
            Color(std::string(color))
        {
        }

        constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF) noexcept:
            v{red, green, blue, alpha}
        {
        }

        explicit Color(float color[4]):
            v{
                static_cast<uint8_t>(round(color[0] * 255.0F)),
                static_cast<uint8_t>(round(color[1] * 255.0F)),
                static_cast<uint8_t>(round(color[2] * 255.0F)),
                static_cast<uint8_t>(round(color[3] * 255.0F))
            }
        {
        }

        explicit Color(const Vector<3, float>& vec):
            v{
                static_cast<uint8_t>(round(vec.v[0] * 255.0F)),
                static_cast<uint8_t>(round(vec.v[1] * 255.0F)),
                static_cast<uint8_t>(round(vec.v[2] * 255.0F)),
                0
            }
        {
        }

        explicit Color(const Vector<4, float>& vec):
            v{
                static_cast<uint8_t>(round(vec.v[0] * 255.0F)),
                static_cast<uint8_t>(round(vec.v[1] * 255.0F)),
                static_cast<uint8_t>(round(vec.v[2] * 255.0F)),
                static_cast<uint8_t>(round(vec.v[3] * 255.0F))
            }
        {
        }

        static constexpr Color black() { return Color(0, 0, 0, 255); }
        static constexpr Color red() { return Color(255, 0, 0, 255); }
        static constexpr Color magenta() { return Color(255, 0, 255, 255); }
        static constexpr Color green() { return Color(0, 255, 0, 255); }
        static constexpr Color cyan() { return Color(0, 255, 255, 255); }
        static constexpr Color blue() { return Color(0, 0, 255, 255); }
        static constexpr Color yellow() { return Color(255, 255, 0, 255); }
        static constexpr Color white() { return Color(255, 255, 255, 255); }
        static constexpr Color gray() { return Color(128, 128, 128, 255); }

        uint8_t& operator[](size_t index) noexcept { return v[index]; }
        constexpr uint8_t operator[](size_t index) const noexcept { return v[index]; }

        uint8_t& r() noexcept { return v[0]; }
        constexpr uint8_t r() const noexcept { return v[0]; }

        uint8_t& g() noexcept { return v[1]; }
        constexpr uint8_t g() const noexcept { return v[1]; }

        uint8_t& b() noexcept { return v[2]; }
        constexpr uint8_t b() const noexcept { return v[2]; }

        uint8_t& a() noexcept { return v[3]; }
        constexpr uint8_t a() const noexcept { return v[3]; }

        inline std::array<float, 4> norm() const
        {
            return {{v[0] / 255.0F, v[1] / 255.0F, v[2] / 255.0F, v[3] / 255.0F}};
        }
        constexpr float normR() const noexcept { return v[0] / 255.0F; }
        constexpr float normG() const noexcept { return v[1] / 255.0F; }
        constexpr float normB() const noexcept { return v[2] / 255.0F; }
        constexpr float normA() const noexcept { return v[3] / 255.0F; }

        constexpr auto getIntValue() const noexcept
        {
            return (static_cast<uint32_t>(v[0]) << 24) |
                   (static_cast<uint32_t>(v[1]) << 16) |
                   (static_cast<uint32_t>(v[2]) << 8) |
                   static_cast<uint32_t>(v[3]);
        }

        inline bool operator<(const Color& c) const
        {
            for (size_t i = 0; i < 4; ++i)
            {
                if (v[i] < c.v[i]) return true;
                if (c.v[i] < v[i]) return false;
            }

            return false;
        }
        
        inline bool operator==(const Color& c) const
        {
            return v[0] == c.v[0] &&
                v[1] == c.v[1] &&
                v[2] == c.v[2] &&
                v[3] == c.v[3];
        }

        inline bool operator!=(const Color& c) const
        {
            return v[0] != c.v[0] ||
                v[1] != c.v[1] ||
                v[2] != c.v[2] ||
                v[3] != c.v[3];
        }

        inline auto isZero() const noexcept
        {
            return v[0] == 0 &&
                v[1] == 0 &&
                v[2] == 0 &&
                v[3] == 0;
        }
    };
} // namespace ouzel

#endif // OUZEL_MATH_COLOR_HPP
