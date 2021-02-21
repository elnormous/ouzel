// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
#define OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP

namespace ouzel::graphics
{
    enum class SamplerAddressMode
    {
        clampToEdge,
        clampToBorder,
        repeat,
        mirrorRepeat
    };
}

#endif // OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
