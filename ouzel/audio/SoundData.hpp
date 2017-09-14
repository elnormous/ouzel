// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "utils/Noncopyable.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;
        class Stream;

        class SoundData: public Noncopyable
        {
            friend Audio;
        public:
            SoundData();
            virtual ~SoundData();

            virtual bool init(const std::string& newFilename);
            virtual bool init(const std::vector<uint8_t>& newData);

            virtual std::shared_ptr<Stream> createStream() = 0;
            bool getData(Stream* stream, uint32_t frames, uint32_t neededChannels, uint32_t neededSampleRate, float pitch, std::vector<float>& result);

            uint16_t getChannels() const { return channels; }
            uint32_t getSampleRate() const { return sampleRate; }

        protected:
            virtual bool readData(Stream* stream, uint32_t frames, std::vector<float>& result) = 0;

            uint16_t channels = 0;
            uint32_t sampleRate = 0;

            std::vector<float> tempData;
            std::vector<float> resampledData;
        };
    } // namespace audio
} // namespace ouzel
