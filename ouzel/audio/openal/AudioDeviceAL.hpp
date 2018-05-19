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
#include "utils/Log.hpp"

namespace ouzel
{
    namespace audio
    {
        class AudioDeviceAL: public AudioDevice
        {
            friend Audio;
        public:
            static bool checkOpenALError()
            {
                ALenum error = alGetError();

                if (error != AL_NO_ERROR)
                {
                    const char* errorStr;

                    switch (error)
                    {
                        case AL_INVALID_NAME: errorStr = "AL_INVALID_NAME"; break;
                        case AL_INVALID_ENUM: errorStr = "AL_INVALID_ENUM"; break;
                        case AL_INVALID_VALUE: errorStr = "AL_INVALID_VALUE"; break;
                        case AL_INVALID_OPERATION: errorStr = "AL_INVALID_OPERATION"; break;
                        case AL_OUT_OF_MEMORY: errorStr = "AL_OUT_OF_MEMORY"; break;
                        default: errorStr = "Unknown error"; break;
                    }

                    Log(Log::Level::ERR) << "OpenAL error: " << errorStr << " (" << error << ")";

                    return true;
                }

                return false;
            }

            bool checkALCError();

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
