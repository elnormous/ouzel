// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEDS_HPP
#define OUZEL_AUDIO_AUDIODEVICEDS_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <atomic>
#include <thread>
#include <dsound.h>
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceDS final: public AudioDevice
        {
        public:
            AudioDeviceDS(Mixer& initMixer, Window* window);
            ~AudioDeviceDS();

            IDirectSound8* getDirectSound() const { return directSound; }

        private:
            void run();

            IDirectSound8* directSound = nullptr;

            IDirectSoundBuffer* primaryBuffer = nullptr;
            IDirectSoundBuffer8* buffer = nullptr;
            IDirectSoundNotify* notify = nullptr;
            HANDLE notifyEvents[2] = {nullptr, nullptr};

            uint32_t nextBuffer = 0;

            uint32_t sampleSize = 0;
            std::vector<uint8_t> data;

            std::atomic_bool running{false};
            std::thread audioThread;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEDS_HPP
