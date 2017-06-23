// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

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

            virtual bool update() override;

            virtual SoundResource* createSound() override;

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

#endif
