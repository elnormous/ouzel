// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_AUDIODEVICEAL_HPP
#define OUZEL_AUDIO_AUDIODEVICEAL_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <atomic>
#include <thread>

#if defined(__APPLE__)
#  include <OpenAl/al.h>
#  include <OpenAl/alc.h>
#else
#  include <AL/al.h>
#  include <AL/alc.h>
#endif
#undef OPENAL

#include "audio/AudioDevice.hpp"

namespace ouzel
{
    namespace audio
    {
        class OALAudioDevice final: public AudioDevice
        {
        public:
            explicit OALAudioDevice(const std::function<void(uint32_t frames, uint16_t channels, uint32_t sampleRate, std::vector<float>& samples)>& initDataGetter);
            ~OALAudioDevice();

            void process();

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

#if !defined(__EMSCRIPTEN__)
            std::atomic_bool running{false};
            std::thread audioThread;
#endif
        };
    } // namespace audio
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_AUDIODEVICEAL_HPP
