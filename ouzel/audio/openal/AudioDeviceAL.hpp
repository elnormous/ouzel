// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

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
        class Audio;

        class AudioDeviceAL: public AudioDevice
        {
            friend Audio;
        public:
            bool checkALCError(bool logError = true);
            static bool checkOpenALError(bool logError = true);

            virtual ~AudioDeviceAL();

            virtual bool process() override;

            ALCdevice* getDevice() const { return device; }
            ALCcontext* getContext() const { return context; }

        protected:
            AudioDeviceAL();
            virtual bool init(bool debugAudio) override;

            void run();

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;

            ALenum format40 = 0;
            ALenum format51 = 0;
            ALenum format61 = 0;
            ALenum format71 = 0;

            ALuint sourceId = 0;
            ALenum sampleFormat = 0;
            uint32_t nextBuffer = 0;
            ALuint buffers[2];

            std::vector<uint8_t> data;

#if OUZEL_MULTITHREADED
            std::atomic<bool> running;
            std::thread audioThread;
#endif
        };
    } // namespace audio
} // namespace ouzel

#endif
