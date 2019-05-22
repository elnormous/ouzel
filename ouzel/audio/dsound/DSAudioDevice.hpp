// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEDS_HPP
#define OUZEL_AUDIO_AUDIODEVICEDS_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <atomic>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <mmreg.h>
#include <dsound.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class DSAudioDevice final: public AudioDevice
        {
        public:
            DSAudioDevice(uint32_t initBufferSize,
                          uint32_t initSampleRate,
                          uint16_t initChannels,
                          const std::function<void(uint32_t frames,
                                                   uint16_t channels,
                                                   uint32_t sampleRate,
                                                   std::vector<float>& samples)>& initDataGetter,
                          Window* window);
            ~DSAudioDevice();

            void start() final;
            void stop() final;

            ALWAYSINLINE IDirectSound8* getDirectSound() const { return directSound; }

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
