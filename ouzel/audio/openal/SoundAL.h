// Copyright (C) 2017 Elviss Strazdins
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

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class SoundAL: public Sound
        {
        public:
            SoundAL();
            virtual ~SoundAL();

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual void setPosition(const Vector3& newPosition) override;
            virtual void setPitch(float newPitch) override;
            virtual void setGain(float newGain) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

        protected:
            ALuint sourceId = 0;
            ALuint outputBuffer = 0;
        };
    } // namespace audio
} // namespace ouzel
