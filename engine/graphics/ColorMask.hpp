// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_COLORMASK_HPP
#define OUZEL_GRAPHICS_COLORMASK_HPP

#include <type_traits>

namespace ouzel::graphics
{
    enum class ColorMask: std::uint8_t
    {
        none = 0x00,
        red = 0x01,
        green = 0x02,
        blue = 0x04,
        alpha = 0x08,
        all = red | green | blue | alpha
    };

    [[nodiscard]] inline constexpr ColorMask operator&(const ColorMask a, const ColorMask b) noexcept
    {
        return static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) & static_cast<std::underlying_type_t<ColorMask>>(b));
    }
    [[nodiscard]] inline constexpr ColorMask operator|(const ColorMask a, const ColorMask b) noexcept
    {
        return static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) | static_cast<std::underlying_type_t<ColorMask>>(b));
    }
    [[nodiscard]] inline constexpr ColorMask operator^(const ColorMask a, const ColorMask b) noexcept
    {
        return static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) ^ static_cast<std::underlying_type_t<ColorMask>>(b));
    }
    [[nodiscard]] inline constexpr ColorMask operator~(const ColorMask a) noexcept
    {
        return static_cast<ColorMask>(~static_cast<std::underlying_type_t<ColorMask>>(a));
    }
    inline constexpr ColorMask& operator&=(ColorMask& a, const ColorMask b) noexcept
    {
        return a = static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) & static_cast<std::underlying_type_t<ColorMask>>(b));
    }
    inline constexpr ColorMask& operator|=(ColorMask& a, const ColorMask b) noexcept
    {
        return a = static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) | static_cast<std::underlying_type_t<ColorMask>>(b));
    }
    inline constexpr ColorMask& operator^=(ColorMask& a, const ColorMask b) noexcept
    {
        return a = static_cast<ColorMask>(static_cast<std::underlying_type_t<ColorMask>>(a) ^ static_cast<std::underlying_type_t<ColorMask>>(b));
    }
}

#endif // OUZEL_GRAPHICS_COLORMASK_HPP
