#ifndef FLUXION_STORAGE_STORAGEERROR_HPP
#define FLUXION_STORAGE_STORAGEERROR_HPP

#include <stdexcept>

namespace fluxion::storage
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_STORAGE_STORAGEERROR_HPP
