// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_ALCERRORCATEGORY_HPP
#define OUZEL_AUDIO_ALCERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <system_error>
#ifdef __APPLE__
#  include <OpenAl/alc.h>
#else
#  include <AL/alc.h>
#endif

namespace ouzel::audio::alc
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "ALC";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case ALC_INVALID_DEVICE: return "ALC_INVALID_DEVICE";
                case ALC_INVALID_CONTEXT: return "ALC_INVALID_CONTEXT";
                case ALC_INVALID_ENUM: return "ALC_INVALID_ENUM";
                case ALC_INVALID_VALUE: return "ALC_INVALID_VALUE";
                case ALC_OUT_OF_MEMORY: return "ALC_OUT_OF_MEMORY";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_ALCERRORCATEGORY_HPP
