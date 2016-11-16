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

        SoundDataPtr AudioEmpty::createSoundData()
        {
            SoundDataPtr soundData = std::make_shared<SoundDataEmpty>();
            return soundData;
        }

        SoundPtr AudioEmpty::createSound()
        {
            SoundPtr sound = std::make_shared<SoundEmpty>();
            return sound;
        }
    } // namespace audio
} // namespace ouzel
