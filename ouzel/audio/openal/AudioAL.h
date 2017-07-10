// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENAL

#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <OpenAl/al.h>
#include <OpenAl/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#undef OPENAL

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL: public Audio
        {
            friend Engine;
        public:
            bool checkALCError(bool logError = true);
            static bool checkOpenALError(bool logError = true);

            virtual ~AudioAL();

            virtual bool update() override;

            virtual SoundResource* createSound() override;

            ALCdevice* getDevice() const { return device; }
            ALCcontext* getContext() const { return context; }

        protected:
            AudioAL();
            virtual bool init() override;

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;

            ALenum format40 = 0;
            ALenum format51 = 0;
            ALenum format61 = 0;
            ALenum format71 = 0;

            ALuint sourceId = 0;
            ALenum format = 0;
            uint32_t nextBuffer = 0;
            ALuint buffers[2];
        };
    } // namespace audio
} // namespace ouzel

#endif
