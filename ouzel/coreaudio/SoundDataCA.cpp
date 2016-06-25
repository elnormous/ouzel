// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundDataCA.h"

namespace ouzel
{
    namespace audio
    {
        SoundDataCA::SoundDataCA()
        {
        }

        SoundDataCA::~SoundDataCA()
        {
        }

        bool SoundDataCA::initFromBuffer(const std::vector<uint8_t>& newData)
        {
            if (!SoundData::initFromBuffer(newData))
            {
                return false;
            }

            streamDescription.mSampleRate = samplesPerSecond;
            streamDescription.mFormatID = kAudioFormatLinearPCM;
            streamDescription.mFormatFlags = kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
            streamDescription.mChannelsPerFrame = channels;
            streamDescription.mFramesPerPacket = 1;
            streamDescription.mBitsPerChannel = bitsPerSample;
            streamDescription.mBytesPerFrame = streamDescription.mBitsPerChannel * streamDescription.mChannelsPerFrame / 8;
            streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
            streamDescription.mReserved = 0;
            
            return true;
        }

    } // namespace audio
} // namespace ouzel
