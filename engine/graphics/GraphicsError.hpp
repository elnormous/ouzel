#ifndef FLUXION_GRAPHICS_GRAPHICSERROR_HPP
#define FLUXION_GRAPHICS_GRAPHICSERROR_HPP

#include <stdexcept>

namespace fluxion::graphics
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_GRAPHICS_GRAPHICSERROR_HPP
