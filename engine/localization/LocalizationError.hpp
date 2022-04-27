// Ouzel by Elviss Strazdins

#ifndef OUZEL_LOCALIZATION_LOCALIZATIONERROR_HPP
#define OUZEL_LOCALIZATION_LOCALIZATIONERROR_HPP

#include <stdexcept>

namespace ouzel
{
    class LocalizationError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif // OUZEL_LOCALIZATION_LOCALIZATIONERROR_HPP
