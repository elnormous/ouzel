// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataXA2.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataXA2::SoundDataXA2()
        {
        }

        SoundDataXA2::~SoundDataXA2()
        {
        }

        bool SoundDataXA2::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            if (!SoundData::initFromBuffer(newData))
            {
                return false;
            }

            waveFormat.wFormatTag = formatTag;
            waveFormat.nChannels = channels;
            waveFormat.nSamplesPerSec = samplesPerSecond;
            waveFormat.nAvgBytesPerSec = averageBytesPerSecond;
            waveFormat.nBlockAlign = blockAlign;
            waveFormat.wBitsPerSample = bitsPerSample;
            waveFormat.cbSize = 16;

            return true;
        }
    } // namespace audio
} // namespace ouzel
