// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioEmpty.h"
#include "SoundDataEmpty.h"
#include "SoundEmpty.h"

namespace ouzel
{
    namespace audio
    {
        AudioEmpty::AudioEmpty():
            Audio(Driver::EMPTY)
        {
        }

        AudioEmpty::~AudioEmpty()
        {
        }

        SoundDataPtr AudioEmpty::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataEmpty());
            return soundData;
        }

        SoundPtr AudioEmpty::createSound()
        {
            SoundPtr sound(new SoundEmpty());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
