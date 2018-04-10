// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENAL

#include <atomic>

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <OpenAl/al.h>
#include <OpenAl/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#undef OPENAL

#include "audio/AudioDevice.hpp"
#include "thread/Thread.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceAL: public AudioDevice
        {
            friend Audio;
        public:
            bool checkALCError(bool logError = true);
            static bool checkOpenALError(bool logError = true);

            virtual ~AudioDeviceAL();

            virtual bool process() override;

            inline ALCdevice* getDevice() const { return device; }
            inline ALCcontext* getContext() const { return context; }

        protected:
            AudioDeviceAL();
            virtual bool init(bool debugAudio) override;

            void run();

        private:
            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;

            ALenum format40 = 0;
            ALenum format51 = 0;
            ALenum format61 = 0;
            ALenum format71 = 0;

            ALuint sourceId = 0;
            ALenum format = 0;
            uint32_t nextBuffer = 0;
            ALuint bufferIds[2];

            std::vector<uint8_t> data;

#if OUZEL_MULTITHREADED
            std::atomic<bool> running;
            Thread audioThread;
#endif
        };
    } // namespace audio
} // namespace ouzel

#endif
