// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_ALSAERRORCATEGORY_HPP
#define OUZEL_AUDIO_ALSAERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_ALSA

#include <system_error>
#include <alsa/asoundlib.h>

namespace ouzel::audio::alsa
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
}
#endif

#endif // OUZEL_AUDIO_ALSAERRORCATEGORY_HPP
