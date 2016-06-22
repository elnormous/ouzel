// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "SoundCA.h"

namespace ouzel
{
    namespace audio
    {
        SoundCA::SoundCA()
        {
        }

        SoundCA::~SoundCA()
        {
        }

        bool SoundCA::init(const SoundDataPtr& newSoundData)
        {
            if (!Sound::init(newSoundData))
            {
                return false;
            }

            AudioStreamBasicDescription desc;

            memset(&desc, '\0', sizeof(AudioStreamBasicDescription));
            desc.mFormatID = kAudioFormatLinearPCM;
            desc.mFormatFlags = kLinearPCMFormatFlagIsPacked;
            //desc.mChannelsPerFrame = this->spec.channels;
            //desc.mSampleRate = this->spec.freq;
            desc.mFramesPerPacket = 1;

            return true;
        }
    } // namespace audio
} // namespace ouzel
