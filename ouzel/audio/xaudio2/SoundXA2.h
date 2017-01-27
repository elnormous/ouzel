// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class SoundXA2: public Sound
        {
        public:
            SoundXA2();
            virtual ~SoundXA2();

            virtual bool init(const SoundDataPtr& newSoundData) override;

            virtual bool play(bool repeatSound = false) override;
            virtual bool stop(bool resetSound = false) override;
            virtual bool reset() override;

        protected:
            IXAudio2SourceVoice* sourceVoice = nullptr;
        };
    } // namespace audio
} // namespace ouzel
