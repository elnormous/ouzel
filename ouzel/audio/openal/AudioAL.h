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

            ALenum getFormatForChannels(uint16_t channels)
            {
                switch (channels)
                {
                    case 1: return AL_FORMAT_MONO16;
                    case 2: return AL_FORMAT_STEREO16;
                    case 4: return format40;
                    case 6: return format51;
                    case 7: return format61;
                    case 8: return format71;
                    default: return 0;
                }
            }

        protected:
            AudioAL();
            virtual bool init() override;

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;

            ALenum format40 = 0;
            ALenum format51 = 0;
            ALenum format61 = 0;
            ALenum format71 = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif
