#ifndef FLUXION_GRAPHICS_RASTERIZERSTATE_HPP
#define FLUXION_GRAPHICS_RASTERIZERSTATE_HPP

namespace fluxion::graphics
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

#endif // FLUXION_GRAPHICS_RASTERIZERSTATE_HPP
