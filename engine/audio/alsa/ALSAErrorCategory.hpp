#ifndef FLUXION_AUDIO_ALSAERRORCATEGORY_HPP
#define FLUXION_AUDIO_ALSAERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if FLUXION_COMPILE_ALSA

#include <system_error>
#include <alsa/asoundlib.h>

namespace fluxion::audio::alsa
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "ALSA";
        }

        std::string message(const int condition) const override
        {
            return snd_strerror(condition);
        }
    };

    inline const ErrorCategory errorCategory;
}
#endif

#endif // FLUXION_AUDIO_ALSAERRORCATEGORY_HPP
