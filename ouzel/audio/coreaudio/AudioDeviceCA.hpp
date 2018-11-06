// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef AUDIODEVICECA_HPP
#define AUDIODEVICECA_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_COREAUDIO

#if OUZEL_PLATFORM_MACOS
#include <CoreAudio/CoreAudio.h>
#endif

#include <AudioUnit/AudioUnit.h>

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceCA final: public AudioDevice
        {
            friend Audio;
        public:
            ~AudioDeviceCA();

            void outputCallback(AudioBufferList* ioData);

        protected:
            AudioDeviceCA();

        private:
#if OUZEL_PLATFORM_MACOS
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

#endif // AUDIODEVICECA_HPP
