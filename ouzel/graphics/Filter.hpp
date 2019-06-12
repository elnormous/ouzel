// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_FILTER_HPP
#define OUZEL_GRAPHICS_FILTER_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class Filter
        {
            DEFAULT,
            POINT,
            LINEAR,
            BILINEAR,
            TRILINEAR
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_FILTER_HPP
