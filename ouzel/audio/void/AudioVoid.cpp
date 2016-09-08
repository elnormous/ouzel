// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioVoid.h"
#include "SoundDataVoid.h"
#include "SoundVoid.h"

namespace ouzel
{
    namespace audio
    {
        AudioVoid::AudioVoid():
            Audio(Driver::VOID)
        {
        }

        AudioVoid::~AudioVoid()
        {
        }

        SoundDataPtr AudioVoid::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataVoid());
            return soundData;
        }

        SoundPtr AudioVoid::createSound()
        {
            SoundPtr sound(new SoundVoid());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
