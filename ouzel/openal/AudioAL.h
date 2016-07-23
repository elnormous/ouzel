// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

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
            static bool checkOpenALErrors(bool logError = true);
            
            virtual ~AudioAL();
            virtual void free() override;

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

            ALCdevice* getDevice() const { return device; }
            ALCcontext* getContext() const { return context; }

        protected:
            AudioAL();

            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
        };
    } // namespace audio
} // namespace ouzel
