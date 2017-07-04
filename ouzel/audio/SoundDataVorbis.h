// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "audio/SoundData.h"

struct stb_vorbis;

namespace ouzel
{
    namespace audio
    {
        class SoundDataVorbis: public SoundData
        {
        public:
            SoundDataVorbis();
            virtual ~SoundDataVorbis();

            virtual bool init(const std::string& newFilename);
            virtual bool init(const std::vector<uint8_t>& newData);

            virtual std::unique_ptr<Stream> createStream() override;
            virtual std::vector<uint8_t> getData(Stream* stream = nullptr, uint32_t size = 0) override;

        protected:
            std::string filename;

            std::vector<uint8_t> data;
            stb_vorbis* vorbisStream = nullptr;
        };
    } // namespace audio
} // namespace ouzel
