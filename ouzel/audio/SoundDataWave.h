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
        class SoundDataWave: public SoundData
        {
        public:
            SoundDataWave();

            virtual bool init(const std::string& newFilename) override;
            virtual bool init(const std::vector<uint8_t>& newData) override;

            virtual std::unique_ptr<Stream> createStream() override;
            virtual bool getData(Stream* stream, uint32_t samples, std::vector<float>& result) override;

        protected:
            std::string filename;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel
