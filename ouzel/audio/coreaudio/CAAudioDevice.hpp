// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICECA_HPP
#define OUZEL_AUDIO_AUDIODEVICECA_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
#  include <CoreAudio/CoreAudio.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class CAAudioDevice final: public AudioDevice
        {
        public:
            explicit CAAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
            ~CAAudioDevice();

            void outputCallback(AudioBufferList* ioData);

        private:
#if TARGET_OS_MAC && !TARGET_OS_IOS && !TARGET_OS_TV
            AudioDeviceID deviceId = 0;
#endif
            AudioComponent audioComponent = nullptr;
            AudioUnit audioUnit = nullptr;

            uint32_t sampleSize = 0;
            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICECA_HPP
