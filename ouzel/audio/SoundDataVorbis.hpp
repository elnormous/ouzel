// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "audio/SoundData.hpp"

namespace ouzel
{
    namespace audio
    {
        class SoundDataVorbis: public SoundData
        {
        public:
            SoundDataVorbis();

            virtual void init(const std::vector<uint8_t>& newData) override;

            virtual std::shared_ptr<Stream> createStream() override;

        protected:
            virtual bool readData(Stream* stream, uint32_t frames, std::vector<float>& result) override;

            std::vector<uint8_t> data;
        };
    } // namespace audio
} // namespace ouzel
