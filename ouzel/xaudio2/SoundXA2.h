// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/Sound.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2;

        class SoundXA2: public Sound
        {
            friend AudioXA2;
        public:
            virtual ~SoundXA2();

            virtual bool init(const SoundDataPtr& newSoundData) override;

        protected:
            SoundXA2();

            IXAudio2SourceVoice* sourceVoice = nullptr;
        };
    } // namespace audio
} // namespace ouzel
