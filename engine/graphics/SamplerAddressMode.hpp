// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

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
