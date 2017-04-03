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

        protected:
            std::string filename;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel
