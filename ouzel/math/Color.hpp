// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_COLOR_HPP
#define OUZEL_MATH_COLOR_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace ouzel
{
    template<class T> class Vector3;
    template<class T> class Vector4;

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

        uint8_t v[4]{0, 0, 0 , 0};

        Color()
        {
        }

        Color(uint32_t color):
            v{
                static_cast<uint8_t>((color & 0xFF000000) >> 24),
                static_cast<uint8_t>((color & 0x00FF0000) >> 16),
                static_cast<uint8_t>((color & 0x0000FF00) >> 8),
                static_cast<uint8_t>(color & 0x000000FF)
            }
        {
        }

        Color(const std::string& color);

        Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF):
            v{red, green, blue, alpha}
        {
        }

        explicit Color(float color[4]):
            v{
                static_cast<uint8_t>(color[0] * 255),
                static_cast<uint8_t>(color[1] * 255),
                static_cast<uint8_t>(color[2] * 255),
                static_cast<uint8_t>(color[3] * 255)
            }
        {
        }

        explicit Color(const Vector3<float>& vec);

        explicit Color(const Vector4<float>& vec);

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
    };
} // namespace ouzel

#endif // OUZEL_MATH_COLOR_HPP
