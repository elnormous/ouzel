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
            virtual ~SoundData();

            virtual bool initFromFile(const std::string& newFilename);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);

            const std::vector<uint8_t>& getData() const { return data; }

            uint16_t getFormatTag() const { return formatTag; }
            uint16_t getChannels() const { return channels; }
            uint32_t getSamplesPerSecond() const { return samplesPerSecond; }
            uint32_t getAverageBytesPerSecond() const { return averageBytesPerSecond; }
            uint16_t getBlockAlign() const { return blockAlign; }
            uint16_t getBitsPerSample() const { return bitsPerSample; }

            bool isReady() const { return ready; }

        protected:
            SoundData();

            std::string filename;

            uint16_t formatTag = 0;
            uint16_t channels = 0;
            uint32_t samplesPerSecond = 0;
            uint32_t averageBytesPerSecond = 0;
            uint16_t blockAlign = 0;
            uint16_t bitsPerSample = 0;

            std::vector<uint8_t> data;

            bool ready = false;
        };
    } // namespace audio
} // namespace ouzel
