// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
#define OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class SamplerAddressMode
        {
            ClampToEdge,
            ClampToBorder,
            Repeat,
            MirrorRepeat
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_SAMPLERADDRESSMODE_HPP
