// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_CAERRORCATEGORY_HPP
#define OUZEL_AUDIO_CAERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#include <system_error>
#include <CoreAudio/CoreAudioTypes.h>

namespace ouzel::audio::coreaudio
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "CoreAudio";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case kAudio_UnimplementedError: return "kAudio_UnimplementedError";
                case kAudio_FileNotFoundError: return "kAudio_FileNotFoundError";
                case kAudio_FilePermissionError: return "kAudio_FilePermissionError";
                case kAudio_TooManyFilesOpenError: return "kAudio_TooManyFilesOpenError";
                case kAudio_BadFilePathError: return "kAudio_BadFilePathError";
                case kAudio_ParamError: return "kAudio_ParamError";
                case kAudio_MemFullError: return "kAudio_MemFullError";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_CAERRORCATEGORY_HPP
