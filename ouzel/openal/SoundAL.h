// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioAL;

        class SoundAL: public Sound
        {
            friend AudioAL;
        public:
            virtual ~SoundAL();
            virtual void free() override;

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

        protected:
            SoundAL();

            ALuint sourceId = 0;
            ALuint outputBuffer = 0;
        };
    } // namespace audio
} // namespace ouzel
