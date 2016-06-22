// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioXA2.h"
#include "SoundDataXA2.h"
#include "SoundXA2.h"

namespace ouzel
{
    namespace audio
    {
        AudioXA2::AudioXA2()
        {
        }

        AudioXA2::~AudioXA2()
        {
        }

        bool AudioXA2::init()
        {
            return true;
        }

        SoundDataPtr AudioXA2::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataXA2());
            return soundData;
        }

        SoundPtr AudioXA2::createSound()
        {
            SoundPtr sound(new SoundXA2());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
