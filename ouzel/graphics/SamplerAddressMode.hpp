// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
#define OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class SamplerAddressMode
        {
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER,
            REPEAT,
            MIRROR_REPEAT
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
