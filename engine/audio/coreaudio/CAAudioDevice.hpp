// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CAAUDIODEVICE_HPP
#define OUZEL_AUDIO_CAAUDIODEVICE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
#  include <CoreAudio/CoreAudio.h>
#endif

#if !defined(__OBJC__)
#  include <objc/objc.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace coreaudio
        {
            class AudioDevice final: public audio::AudioDevice
            {
            public:
                AudioDevice(uint32_t initBufferSize,
                            uint32_t initSampleRate,
                            uint32_t initChannels,
                            const std::function<void(uint32_t frames,
                                                     uint32_t channels,
                                                     uint32_t sampleRate,
                                                     std::vector<float>& samples)>& initDataGetter);
                ~AudioDevice();

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

                uint32_t sampleSize = 0;
                std::vector<uint8_t> data;
            };
        } // namespace coreaudio
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_CAAUDIODEVICE_HPP
