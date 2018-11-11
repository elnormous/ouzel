// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
#define OUZEL_GRAPHICS_RASTERIZERSTATE_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class CullMode
        {
            NONE,
            FRONT,
            BACK
        };

        enum class FillMode
        {
            SOLID,
            WIREFRAME
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
