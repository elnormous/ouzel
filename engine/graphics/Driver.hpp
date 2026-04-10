#ifndef FLUXION_GRAPHICS_DRIVER_HPP
#define FLUXION_GRAPHICS_DRIVER_HPP

namespace fluxion::graphics
{
    enum class Driver
    {
        empty,
        openGl,
        direct3D11,
        metal
    };
}

#endif // FLUXION_GRAPHICS_DRIVER_HPP
