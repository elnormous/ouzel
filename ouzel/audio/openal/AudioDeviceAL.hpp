// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEAL_HPP
#define OUZEL_AUDIO_AUDIODEVICEAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <atomic>
#include <thread>

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <OpenAl/al.h>
#include <OpenAl/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#undef OPENAL

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceAL final: public AudioDevice
        {
        public:
            AudioDeviceAL();
            ~AudioDeviceAL();

            void process() override;

            inline ALCdevice* getDevice() const { return device; }
            inline ALCcontext* getContext() const { return context; }

        private:
            void run();

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;

            ALenum format40 = 0;
            ALenum format51 = 0;
            ALenum format61 = 0;
            ALenum format71 = 0;

            ALuint sourceId = 0;
            ALenum format = 0;
            uint32_t nextBuffer = 0;
            ALuint bufferIds[2] = {0, 0};

            uint32_t sampleSize = 0;
            std::vector<uint8_t> data;

#if OUZEL_MULTITHREADED
            std::atomic_bool running;
            std::thread audioThread;
#endif
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEAL_HPP
