// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
        class AudioDeviceDS: public AudioDevice
        {
            friend Audio;
        public:
            virtual ~AudioDeviceDS();

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

            std::atomic<bool> running;
            std::thread audioThread;
        };
    } // namespace audio
} // namespace ouzel

#endif
