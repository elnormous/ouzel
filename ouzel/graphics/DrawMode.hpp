// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef DRAWMODE_HPP
#define DRAWMODE_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class DrawMode
        {
            POINT_LIST,
            LINE_LIST,
            LINE_STRIP,
            TRIANGLE_LIST,
            TRIANGLE_STRIP
        };
    } // namespace graphics
} // namespace ouzel

#endif // DRAWMODE_HPP
