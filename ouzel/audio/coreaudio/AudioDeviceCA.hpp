// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

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
        class AudioDeviceCA: public AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDeviceCA();

            void outputCallback(AudioBufferList* ioData);

        protected:
            AudioDeviceCA();
            virtual void init(bool debugAudio) override;

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
