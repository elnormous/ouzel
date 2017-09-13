// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_DIRECTSOUND

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
            AudioDeviceDS();
            virtual bool init(bool debugAudio) override;

            void run();

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
