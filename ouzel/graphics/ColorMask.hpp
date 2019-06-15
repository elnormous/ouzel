// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COLORMASK_HPP
#define OUZEL_GRAPHICS_COLORMASK_HPP

namespace ouzel
{
    namespace graphics
    {
        enum ColorMask
        {
            COLOR_MASK_RED = 0x01,
            COLOR_MASK_GREEN = 0x02,
            COLOR_MASK_BLUE = 0x04,
            COLOR_MASK_ALPHA = 0x08,
            COLOR_MASK_ALL = COLOR_MASK_RED | COLOR_MASK_GREEN | COLOR_MASK_BLUE | COLOR_MASK_ALPHA
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COLORMASK_HPP
