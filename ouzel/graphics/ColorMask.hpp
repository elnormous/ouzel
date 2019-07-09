// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_COLORMASK_HPP
#define OUZEL_GRAPHICS_COLORMASK_HPP

namespace ouzel
{
    namespace graphics
    {
        enum ColorMask
        {
            Red = 0x01,
            Green = 0x02,
            Blue = 0x04,
            Alpha = 0x08,
            All = Red | Green | Blue | Alpha
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_COLORMASK_HPP
