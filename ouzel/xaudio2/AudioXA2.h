// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/Audio.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2: public Audio
        {
            friend Engine;
        public:
            virtual ~AudioXA2();

            virtual bool init() override;

            virtual SoundDataPtr createSoundData() override;
            virtual SoundPtr createSound() override;

            IXAudio2* getXAudio() const { return xAudio; }

        protected:
            AudioXA2();

            IXAudio2* xAudio = nullptr;
            IXAudio2MasteringVoice* masteringVoice = nullptr;
        };
    } // namespace audio
} // namespace ouzel
