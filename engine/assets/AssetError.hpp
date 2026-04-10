#ifndef FLUXION_ASSETS_ASSETERROR_HPP
#define FLUXION_ASSETS_ASSETERROR_HPP

#include <stdexcept>

namespace fluxion::assets
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_ASSETS_ASSETERROR_HPP
