// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_XA2ERRORCATEGORY_HPP
#define OUZEL_AUDIO_XA2ERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <system_error>
#include <xaudio2.h>

namespace ouzel::audio::xaudio2
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "XAudio2";
        }

        std::string message(int condition) const override
        {
            switch (condition)
            {
                case XAUDIO2_E_INVALID_CALL: return "XAUDIO2_E_INVALID_CALL";
                case XAUDIO2_E_XMA_DECODER_ERROR: return "XAUDIO2_E_XMA_DECODER_ERROR";
                case XAUDIO2_E_XAPO_CREATION_FAILED: return "XAUDIO2_E_XAPO_CREATION_FAILED";
                case XAUDIO2_E_DEVICE_INVALIDATED: return "XAUDIO2_E_DEVICE_INVALIDATED";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_XA2ERRORCATEGORY_HPP
