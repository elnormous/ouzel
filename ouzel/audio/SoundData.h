// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class SoundData
        {
            friend Audio;
        public:
            SoundData();
            virtual ~SoundData();

            virtual const std::vector<uint8_t>& getData() = 0;

            uint16_t getFormatTag() const { return formatTag; }
            uint16_t getChannels() const { return channels; }
            uint32_t getSamplesPerSecond() const { return samplesPerSecond; }

        protected:
            uint16_t formatTag = 0;
            uint16_t channels = 0;
            uint32_t samplesPerSecond = 0;
        };
    } // namespace audio
} // namespace ouzel
