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
            friend Audio;
        public:
            ~AudioDeviceDS();

            IDirectSound8* getDirectSound() const { return directSound; }

        protected:
            AudioDeviceDS(Window* window);

            void run();

        private:
            IDirectSound8* directSound = nullptr;

            IDirectSoundBuffer* primaryBuffer = nullptr;
            IDirectSoundBuffer8* buffer = nullptr;
            IDirectSoundNotify* notify = nullptr;
            HANDLE notifyEvents[2];

            uint32_t nextBuffer = 0;

            std::vector<uint8_t> data;

            std::atomic_bool running;
            std::thread audioThread;
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEDS_HPP
