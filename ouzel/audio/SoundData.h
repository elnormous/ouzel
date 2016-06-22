// Copyright (C) 2016 Elviss Strazdins
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

        protected:
            SoundData();

            std::string filename;
        };
    } // namespace audio
} // namespace ouzel
