// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <vector>
#include "audio/SoundData.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundDataWave final: public SoundData
        {
        public:
            SoundDataWave();
            SoundDataWave(const std::vector<uint8_t>& initData);

            virtual std::shared_ptr<Stream> createStream() override;

        protected:
            virtual void readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<float> data;
        };
    } // namespace audio
} // namespace ouzel
