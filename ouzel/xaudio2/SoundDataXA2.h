// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class AudioXA2;

        class SoundDataXA2: public SoundData
        {
            friend AudioXA2;
        public:
            virtual ~SoundDataXA2();
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData) override;

            const WAVEFORMATEX& getWaveFormat() const { return waveFormat; }

        protected:
            SoundDataXA2();

            WAVEFORMATEX waveFormat;
        };
    } // namespace audio
} // namespace ouzel
