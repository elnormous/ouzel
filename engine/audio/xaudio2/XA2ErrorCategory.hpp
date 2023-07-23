// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_XA2ERRORCATEGORY_HPP
#define OUZEL_AUDIO_XA2ERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_XAUDIO2

#include <system_error>
#include <xaudio2.h>

namespace ouzel::audio::xaudio2
{
    enum ErrorCode {
        ErrorInvalidCall,
        ErrorXmaDeviceError,
        ErrorXapoCreationFailed,
        ErrorDeviceInvalidated
    };

    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "XAudio2";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case ErrorCode::ErrorInvalidCall: return "XAUDIO2_E_INVALID_CALL";
                case ErrorCode::ErrorXmaDeviceError: return "XAUDIO2_E_XMA_DECODER_ERROR";
                case ErrorCode::ErrorXapoCreationFailed: return "XAUDIO2_E_XAPO_CREATION_FAILED";
                case ErrorCode::ErrorDeviceInvalidated: return "XAUDIO2_E_DEVICE_INVALIDATED";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    inline ErrorCode toErrorCode(HRESULT hr) {
        switch (hr) {
            case XAUDIO2_E_INVALID_CALL:
                return ErrorCode::ErrorInvalidCall;

            case XAUDIO2_E_XMA_DECODER_ERROR:
                return ErrorCode::ErrorXmaDeviceError;

            case XAUDIO2_E_XAPO_CREATION_FAILED:
                return ErrorCode::ErrorXapoCreationFailed;
            
            case XAUDIO2_E_DEVICE_INVALIDATED:
                return ErrorCode::ErrorDeviceInvalidated;
            
            default:
                return static_cast<ErrorCode>(hr);
        }
    }

    inline const ErrorCategory errorCategory;
}
#endif

#endif // OUZEL_AUDIO_XA2ERRORCATEGORY_HPP
