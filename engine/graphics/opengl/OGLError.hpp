#ifndef FLUXION_GRAPHICS_OGLERROR_HPP
#define FLUXION_GRAPHICS_OGLERROR_HPP

#include <stdexcept>

namespace fluxion::graphics::opengl
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_GRAPHICS_OGLERROR_HPP
