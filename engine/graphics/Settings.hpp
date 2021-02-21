// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SETTINGS_HPP
#define OUZEL_GRAPHICS_SETTINGS_HPP

#include <cstdint>
#include "SamplerFilter.hpp"

namespace ouzel::graphics
{
    struct Settings final
    {
        std::uint32_t sampleCount = 1;
        SamplerFilter textureFilter = SamplerFilter::point;
        std::uint32_t maxAnisotropy = 1;
        bool srgb = false;
        bool verticalSync = false;
        bool depth = false;
        bool stencil = false;
        bool debugRenderer = false;
    };
}

#endif // OUZEL_GRAPHICS_SETTINGS_HPP
