// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_FLAGS_HPP
#define OUZEL_GRAPHICS_FLAGS_HPP

#include <cstdint>

namespace ouzel
{
    namespace graphics
    {
        enum Flags
        {
            Dynamic = 0x01,
            BindRenderTarget = 0x02,
            BindShader = 0x04,
            BindShaderMsaa = 0x08
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_FLAGS_HPP
