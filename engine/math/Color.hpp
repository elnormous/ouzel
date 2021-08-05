// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_COLOR_HPP
#define OUZEL_MATH_COLOR_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include "Vector.hpp"

namespace ouzel
{
    class Color final
    {
    public:
        std::array<std::uint8_t, 4> v{};

        constexpr Color() noexcept {}

        explicit constexpr Color(const std::uint32_t color) noexcept:
            v{{
                static_cast<std::uint8_t>((color & 0xFF000000U) >> 24),
                static_cast<std::uint8_t>((color & 0x00FF0000U) >> 16),
                static_cast<std::uint8_t>((color & 0x0000FF00U) >> 8),
                static_cast<std::uint8_t>(color & 0x000000FFU)
            }}
        {
        }

        explicit Color(std::string_view color)
        {
            if (!color.empty())
            {
                if (color.front() == '#')
                {
                    assert(color.length() == 4 || color.length() == 7);

                    const std::size_t componentSize = (color.length() - 1) / 3; // exclude the #

                    for (std::size_t component = 0; component < 3; ++component)
                    {
                        v[component] = 0;

                        for (std::size_t byte = 0; byte < 2; ++byte)
                        {
                            const char c = (byte < componentSize) ? color[component * componentSize + byte + 1] : color[component * componentSize + 1];
                            v[component] = static_cast<std::uint8_t>((v[component] << 4) | hexToInt(c));
                        }
                    }

                    v[3] = static_cast<std::uint8_t>(0xFFU); // alpha
                }
                else
                {
                    std::uint32_t intValue = 0;

                    for (const auto c : color)
                        intValue = intValue * 10 + decToInt(c);

                    v[0] = static_cast<std::uint8_t>((intValue & 0xFF000000U) >> 24);
                    v[1] = static_cast<std::uint8_t>((intValue & 0x00FF0000U) >> 16);
                    v[2] = static_cast<std::uint8_t>((intValue & 0x0000FF00U) >> 8);
                    v[3] = static_cast<std::uint8_t>(intValue & 0x000000FFU);
                }
            }
            else
                for (std::size_t i = 0; i < 4; ++i)
                    v[i] = 0;
        }

        template <typename T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
        constexpr Color(const T red, const T green, const T blue, const T alpha = 0xFF) noexcept:
            v{{std::uint8_t(red), std::uint8_t(green), std::uint8_t(blue), std::uint8_t(alpha)}}
        {
        }

        constexpr Color(const float red, const float green, const float blue, const float alpha = 1.0F) noexcept:
            v{{
                static_cast<std::uint8_t>(red * 255.0F),
                static_cast<std::uint8_t>(green * 255.0F),
                static_cast<std::uint8_t>(blue * 255.0F),
                static_cast<std::uint8_t>(alpha * 255.0F)
            }}
        {
        }

        explicit Color(const float color[4]) noexcept:
            v{{
                static_cast<std::uint8_t>(std::round(color[0] * 255.0F)),
                static_cast<std::uint8_t>(std::round(color[1] * 255.0F)),
                static_cast<std::uint8_t>(std::round(color[2] * 255.0F)),
                static_cast<std::uint8_t>(std::round(color[3] * 255.0F))
            }}
        {
        }

        explicit Color(const Vector<float, 3>& vec) noexcept:
            v{{
                static_cast<std::uint8_t>(std::round(vec.v[0] * 255.0F)),
                static_cast<std::uint8_t>(std::round(vec.v[1] * 255.0F)),
                static_cast<std::uint8_t>(std::round(vec.v[2] * 255.0F)),
                static_cast<std::uint8_t>(0xFFU)
            }}
        {
        }

        explicit Color(const Vector<float, 4>& vec) noexcept:
            v{{
                static_cast<std::uint8_t>(std::round(vec.v[0] * 255.0F)),
                static_cast<std::uint8_t>(std::round(vec.v[1] * 255.0F)),
                static_cast<std::uint8_t>(std::round(vec.v[2] * 255.0F)),
                static_cast<std::uint8_t>(std::round(vec.v[3] * 255.0F))
            }}
        {
        }

        static constexpr auto black() noexcept { return Color{0, 0, 0, 255}; }
        static constexpr auto red() noexcept { return Color{255, 0, 0, 255}; }
        static constexpr auto magenta() noexcept { return Color{255, 0, 255, 255}; }
        static constexpr auto green() noexcept { return Color{0, 255, 0, 255}; }
        static constexpr auto cyan() noexcept { return Color{0, 255, 255, 255}; }
        static constexpr auto blue() noexcept { return Color{0, 0, 255, 255}; }
        static constexpr auto yellow() noexcept { return Color{255, 255, 0, 255}; }
        static constexpr auto white() noexcept { return Color{255, 255, 255, 255}; }
        static constexpr auto gray() noexcept { return Color{128, 128, 128, 255}; }

        auto& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](std::size_t index) const noexcept { return v[index]; }

        auto& r() noexcept { return v[0]; }
        constexpr auto r() const noexcept { return v[0]; }

        auto& g() noexcept { return v[1]; }
        constexpr auto g() const noexcept { return v[1]; }

        auto& b() noexcept { return v[2]; }
        constexpr auto b() const noexcept { return v[2]; }

        auto& a() noexcept { return v[3]; }
        constexpr auto a() const noexcept { return v[3]; }

        constexpr std::array<float, 4> norm() const noexcept
        {
            return {{v[0] / 255.0F, v[1] / 255.0F, v[2] / 255.0F, v[3] / 255.0F}};
        }
        constexpr auto normR() const noexcept { return v[0] / 255.0F; }
        constexpr auto normG() const noexcept { return v[1] / 255.0F; }
        constexpr auto normB() const noexcept { return v[2] / 255.0F; }
        constexpr auto normA() const noexcept { return v[3] / 255.0F; }

        constexpr auto getIntValue() const noexcept
        {
            return (static_cast<std::uint32_t>(v[0]) << 24) |
                   (static_cast<std::uint32_t>(v[1]) << 16) |
                   (static_cast<std::uint32_t>(v[2]) << 8) |
                   static_cast<std::uint32_t>(v[3]);
        }

        constexpr auto operator<(const Color& c) const noexcept
        {
            return v[0] == c.v[0] ?
                v[1] == c.v[1] ?
                    v[2] == c.v[2] ?
                        v[3] == c.v[3] ?
                            false :
                            v[3] < c.v[3] :
                        v[2] < c.v[2] :
                    v[1] < c.v[1] :
                v[0] < c.v[0];
        }

        constexpr auto operator==(const Color& c) const noexcept
        {
            return v[0] == c.v[0] &&
                v[1] == c.v[1] &&
                v[2] == c.v[2] &&
                v[3] == c.v[3];
        }

        constexpr auto operator!=(const Color& c) const noexcept
        {
            return v[0] != c.v[0] ||
                v[1] != c.v[1] ||
                v[2] != c.v[2] ||
                v[3] != c.v[3];
        }

    private:
        static constexpr std::uint8_t hexToInt(const char c)
        {
            return (c >= '0' && c <= '9') ? static_cast<std::uint8_t>(c - '0') :
                (c >= 'a' && c <= 'f') ? static_cast<std::uint8_t>(c - 'a' + 10) :
                (c >= 'A' && c <= 'F') ? static_cast<std::uint8_t>(c - 'A' + 10) :
                throw std::out_of_range("Invalid hex digit");
        }

        static constexpr std::uint8_t decToInt(const char c)
        {
            return (c >= '0' && c <= '9') ? static_cast<std::uint8_t>(c - '0') :
                throw std::out_of_range("Invalid hex digit");
        }
    };
}

#endif // OUZEL_MATH_COLOR_HPP
