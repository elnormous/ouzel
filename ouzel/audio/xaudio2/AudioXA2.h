// Copyright (C) 2017 Elviss Strazdins
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

            virtual void setListenerPosition(const Vector3& newPosition);
            virtual void setListenerOrientation(const Vector3& newOrientation);

            virtual SoundPtr createSound() override;

            IXAudio2SourceVoice* createSourceVoice(const WAVEFORMATEX& sourceFormat);

        protected:
            AudioXA2();
            virtual bool init() override;

            HMODULE xAudio2Library = nullptr;

            IXAudio2* xAudio = nullptr;
            IXAudio2MasteringVoice* masteringVoice = nullptr;
        };
    } // namespace audio
} // namespace ouzel
