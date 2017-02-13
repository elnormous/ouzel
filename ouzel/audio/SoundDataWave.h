// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "audio/SoundData.h"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class SoundDataWave: public SoundData
        {
            friend Audio;
        public:
            SoundDataWave();
            virtual ~SoundDataWave();

            virtual bool initFromFile(const std::string& newFilename);
            virtual bool initFromBuffer(const std::vector<uint8_t>& newData);

            virtual void getData(std::vector<uint8_t>& buffer);

            uint16_t getFormatTag() const { return formatTag; }
            uint16_t getChannels() const { return channels; }
            uint32_t getSamplesPerSecond() const { return samplesPerSecond; }
            uint32_t getAverageBytesPerSecond() const { return averageBytesPerSecond; }
            uint16_t getBlockAlign() const { return blockAlign; }
            uint16_t getBitsPerSample() const { return bitsPerSample; }

            bool isReady() const { return ready; }

        protected:
            std::string filename;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel
