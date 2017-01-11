// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <xaudio2.h>

#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class SoundDataXA2: public SoundData
        {
        public:
            SoundDataXA2();
            virtual ~SoundDataXA2();
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData) override;

            const WAVEFORMATEX& getWaveFormat() const { return waveFormat; }

        protected:

            WAVEFORMATEX waveFormat;
        };
    } // namespace audio
} // namespace ouzel
