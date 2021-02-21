// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_FLAGS_HPP
#define OUZEL_GRAPHICS_FLAGS_HPP

namespace ouzel::graphics
{
    enum class Flags
    {
        none = 0x00,
        dynamic = 0x01,
        bindRenderTarget = 0x02,
        bindShader = 0x04,
        bindShaderMsaa = 0x08
    };

    inline constexpr Flags operator&(const Flags a, const Flags b) noexcept
    {
        return static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) & static_cast<std::underlying_type_t<Flags>>(b));
    }
    inline constexpr Flags operator|(const Flags a, const Flags b) noexcept
    {
        return static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) | static_cast<std::underlying_type_t<Flags>>(b));
    }
    inline constexpr Flags operator^(const Flags a, const Flags b) noexcept
    {
        return static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) ^ static_cast<std::underlying_type_t<Flags>>(b));
    }
    inline constexpr Flags operator~(const Flags a) noexcept
    {
        return static_cast<Flags>(~static_cast<std::underlying_type_t<Flags>>(a));
    }
    inline constexpr Flags& operator&=(Flags& a, const Flags b) noexcept
    {
        return a = static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) & static_cast<std::underlying_type_t<Flags>>(b));
    }
    inline constexpr Flags& operator|=(Flags& a, const Flags b) noexcept
    {
        return a = static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) | static_cast<std::underlying_type_t<Flags>>(b));
    }
    inline constexpr Flags& operator^=(Flags& a, const Flags b) noexcept
    {
        return a = static_cast<Flags>(static_cast<std::underlying_type_t<Flags>>(a) ^ static_cast<std::underlying_type_t<Flags>>(b));
    }
}

#endif // OUZEL_GRAPHICS_FLAGS_HPP
