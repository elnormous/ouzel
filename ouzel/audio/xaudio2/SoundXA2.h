// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_XAUDIO2

#include <vector>
#include <xaudio2.h>

#include "audio/SoundResource.h"

namespace ouzel
{
    namespace audio
    {
        class SoundXA2: public SoundResource
        {
        public:
            SoundXA2();
            virtual ~SoundXA2();

        protected:
            virtual bool update() override;

            uint32_t channels = 0;
            IXAudio2SourceVoice* sourceVoice = nullptr;
            float gain = 1.0f;
        };
    } // namespace audio
} // namespace ouzel

#endif
