#ifndef FLUXION_GRAPHICS_SETTINGS_HPP
#define FLUXION_GRAPHICS_SETTINGS_HPP

#include <cstdint>
#include "SamplerFilter.hpp"

namespace fluxion::graphics
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

#endif // FLUXION_GRAPHICS_SETTINGS_HPP
