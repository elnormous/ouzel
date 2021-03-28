// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_CAAUDIODEVICE_HPP
#define OUZEL_AUDIO_CAAUDIODEVICE_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#ifdef __APPLE__
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
#  include <CoreAudio/CoreAudio.h>
#endif

#ifndef __OBJC__
#  include <objc/objc.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "../AudioDevice.hpp"

namespace ouzel::audio::coreaudio
{
    class AudioDevice final: public audio::AudioDevice
    {
    public:
        AudioDevice(const Settings& settings,
                    const std::function<void(std::uint32_t frames,
                                             std::uint32_t channels,
                                             std::uint32_t sampleRate,
                                             std::vector<float>& samples)>& initDataGetter);
        ~AudioDevice() override;

        void start() final;
        void stop() final;

        void outputCallback(AudioBufferList* ioData);

    private:
#if TARGET_OS_IOS || TARGET_OS_TV
        id routeChangeDelegate = nil;
#elif TARGET_OS_MAC
        AudioDeviceID deviceId = 0;
#endif
        AudioComponent audioComponent = nullptr;
        AudioUnit audioUnit = nullptr;

        std::uint32_t sampleSize = 0;
        std::vector<std::uint8_t> data;
    };
}
#endif

#endif // OUZEL_AUDIO_CAAUDIODEVICE_HPP
