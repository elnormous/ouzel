// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
#define OUZEL_GRAPHICS_RASTERIZERSTATE_HPP

namespace ouzel
{
    namespace graphics
    {
        enum class CullMode
        {
            None,
            Front,
            Back
        };

        enum class FillMode
        {
            Solid,
            Wireframe
        };

        enum class Face
        {
            ClockWise,
            CounterClockWise
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
