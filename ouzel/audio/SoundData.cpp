// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundData.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        SoundData::SoundData()
        {
        }

        SoundData::~SoundData()
        {
        }

        bool SoundData::init(const std::string&)
        {
            return true;
        }

        bool SoundData::init(const std::vector<uint8_t>&)
        {
            return true;
        }

        bool SoundData::getData(Stream* stream, uint32_t frames, float pitch, std::vector<float>& result)
        {
            uint32_t neededFrames = static_cast<uint32_t>(frames * pitch);

            if (!readData(stream, neededFrames, tempData))
            {
                return false;
            }

            if (pitch != 1.0f)
            {
                uint32_t srcFrames = static_cast<uint32_t>(tempData.size()) / channels;
                uint32_t dstFrames = static_cast<uint32_t>(frames * (static_cast<float>(srcFrames) / neededFrames));

                Audio::resampleLerp(tempData, srcFrames, result, dstFrames, channels);
            }
            else
            {
                result = tempData;
            }

            return true;
        }
    } // namespace audio
} // namespace ouzel
