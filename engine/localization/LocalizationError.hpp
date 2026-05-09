#ifndef FLUXION_LOCALIZATION_LOCALIZATIONERROR_HPP
#define FLUXION_LOCALIZATION_LOCALIZATIONERROR_HPP

#include <stdexcept>

namespace fluxion::l10n
{
    class Error final: public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
}

#endif // FLUXION_LOCALIZATION_LOCALIZATIONERROR_HPP
