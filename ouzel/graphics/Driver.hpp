// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DRIVER_HPP
#define OUZEL_GRAPHICS_DRIVER_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class Driver
        {
            DEFAULT,
            EMPTY,
            OPENGL,
            DIRECT3D11,
            METAL
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DRIVER_HPP
