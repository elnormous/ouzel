// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_DRIVER_HPP
#define OUZEL_GRAPHICS_DRIVER_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class Driver
        {
            empty,
            openGL,
            direct3D11,
            metal
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_DRIVER_HPP
