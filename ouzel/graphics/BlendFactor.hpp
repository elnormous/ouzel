// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_BLENDFACTOR_HPP
#define OUZEL_GRAPHICS_BLENDFACTOR_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class BlendFactor
        {
            ZERO,
            ONE,
            SRC_COLOR,
            INV_SRC_COLOR,
            SRC_ALPHA,
            INV_SRC_ALPHA,
            DEST_ALPHA,
            INV_DEST_ALPHA,
            DEST_COLOR,
            INV_DEST_COLOR,
            SRC_ALPHA_SAT,
            BLEND_FACTOR,
            INV_BLEND_FACTOR
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_BLENDFACTOR_HPP
