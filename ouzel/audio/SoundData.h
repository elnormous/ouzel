// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Stream;

        class SoundData
        {
            friend Audio;
        public:
            SoundData();
            virtual ~SoundData();

            virtual bool init(const std::string& newFilename);
            virtual bool init(const std::vector<uint8_t>& newData);

            virtual std::unique_ptr<Stream> createStream() = 0;
            virtual std::vector<uint8_t> getData(Stream* stream = nullptr, uint32_t size = 0) = 0;

            uint16_t getChannels() const { return channels; }
            uint32_t getSamplesPerSecond() const { return samplesPerSecond; }

        protected:
            uint16_t channels = 0;
            uint32_t samplesPerSecond = 0;
        };
    } // namespace audio
} // namespace ouzel
