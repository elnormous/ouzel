// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_FLAGS_HPP
#define OUZEL_GRAPHICS_FLAGS_HPP

#include <cstdint>
#include "utils/Inline.h"

namespace ouzel
{
    namespace graphics
    {
        enum Flags
        {
            DYNAMIC = 0x01,
            BIND_RENDER_TARGET = 0x02,
            BIND_SHADER = 0x04,
            BIND_SHADER_MSAA = 0x08
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_FLAGS_HPP
