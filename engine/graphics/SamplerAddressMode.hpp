#ifndef FLUXION_GRAPHICS_SAMPLERADDRESSMODE_HPP
#define FLUXION_GRAPHICS_SAMPLERADDRESSMODE_HPP

namespace fluxion::graphics
{
    enum class SamplerAddressMode
    {
        clampToEdge,
        clampToBorder,
        repeat,
        mirrorRepeat
    };
}

#endif // FLUXION_GRAPHICS_SAMPLERADDRESSMODE_HPP
