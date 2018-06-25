// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "audio/SoundData.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundDataWave: public SoundData
        {
        public:
            SoundDataWave();

            virtual void init(const std::vector<uint8_t>& newData) override;

            virtual std::shared_ptr<Stream> createStream() override;

        protected:
            virtual void readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel
