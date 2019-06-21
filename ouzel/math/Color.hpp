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

        constexpr Color() {}

        constexpr Color(uint32_t color):
            v{
                static_cast<uint8_t>((color & 0xFF000000) >> 24),
                static_cast<uint8_t>((color & 0x00FF0000) >> 16),
                static_cast<uint8_t>((color & 0x0000FF00) >> 8),
                static_cast<uint8_t>(color & 0x000000FF)
            }
        {
        }

        Color(const std::string& color)
        {
            if (!color.empty())
            {
                if (color.front() == '#')
                {
                    std::string newValue(color.begin() + 1, color.end());

                    size_t componentSize = (newValue.length() + 2) / 3; // get the size of component rounded up
                    size_t newSize = componentSize * 3;
                    newValue.resize(newSize);

                    for (size_t component = 0; component < 3; ++component)
                    {
                        std::string currentValue;

                        for (size_t byte = 0; byte < ((componentSize < 2) ? componentSize : 2); ++byte)
                        {
                            char c = newValue[component * componentSize + byte];

                            if ((c >= '0' && c <= '9') ||
                                (c >= 'a' && c <= 'f') ||
                                (c >= 'A' && c <= 'F'))
                                currentValue += c;
                            else
                                currentValue += "0";
                        }

                        v[component] = static_cast<uint8_t>(std::stoul(currentValue, 0, 16));
                    }

                    v[3] = 0xFF; // alpha
                }
                else
                {
                    uint32_t intValue = static_cast<uint32_t>(std::stoul(color));
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

        constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF):
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

        inline uint8_t& operator[](size_t index) { return v[index]; }
        inline uint8_t operator[](size_t index) const { return v[index]; }

        inline uint8_t& r() { return v[0]; }
        inline uint8_t r() const { return v[0]; }

        inline uint8_t& g() { return v[1]; }
        inline uint8_t g() const { return v[1]; }

        inline uint8_t& b() { return v[2]; }
        inline uint8_t b() const { return v[2]; }

        inline uint8_t& a() { return v[3]; }
        inline uint8_t a() const { return v[3]; }

        inline std::array<float, 4> norm() const
        {
            return {{v[0] / 255.0F, v[1] / 255.0F, v[2] / 255.0F, v[3] / 255.0F}};
        }
        inline float normR() const { return v[0] / 255.0F; }
        inline float normG() const { return v[1] / 255.0F; }
        inline float normB() const { return v[2] / 255.0F; }
        inline float normA() const { return v[3] / 255.0F; }

        inline uint32_t getIntValue() const
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

        inline bool isZero() const
        {
            return v[0] == 0 &&
                v[1] == 0 &&
                v[2] == 0 &&
                v[3] == 0;
        }
    };
} // namespace ouzel

#endif // OUZEL_MATH_COLOR_HPP
