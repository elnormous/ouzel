#ifndef FLUXION_GRAPHICS_BLENDFACTOR_HPP
#define FLUXION_GRAPHICS_BLENDFACTOR_HPP

namespace fluxion::graphics
{
    enum class BlendFactor
    {
        zero,
        one,
        srcColor,
        invSrcColor,
        srcAlpha,
        invSrcAlpha,
        destAlpha,
        invDestAlpha,
        destColor,
        invDestColor,
        srcAlphaSat,
        blendFactor,
        invBlendFactor
    };
}

#endif // FLUXION_GRAPHICS_BLENDFACTOR_HPP
