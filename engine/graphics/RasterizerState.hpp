// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
#define OUZEL_GRAPHICS_RASTERIZERSTATE_HPP

namespace ouzel::graphics
{
    enum class CullMode
    {
        none,
        front,
        back
    };

    enum class FillMode
    {
        solid,
        wireframe
    };

    enum class Face
    {
        clockWise,
        counterClockWise
    };
}

#endif // OUZEL_GRAPHICS_RASTERIZERSTATE_HPP
