// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_OALERRORCATEGORY_HPP
#define OUZEL_AUDIO_OALERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <system_error>
#ifdef __APPLE__
#  include <OpenAl/al.h>
#else
#  include <AL/al.h>
#endif

namespace ouzel::audio::openal
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "OpenAL";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case AL_INVALID_NAME: return "AL_INVALID_NAME";
                case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
                case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
                case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
                case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    inline const ErrorCategory errorCategory;
}
#endif

#endif // OUZEL_AUDIO_OALERRORCATEGORY_HPP
