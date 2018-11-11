// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RASTERIZERSTATE_HPP
#define RASTERIZERSTATE_HPP

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

#endif // RASTERIZERSTATE_HPP
