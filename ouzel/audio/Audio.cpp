// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Audio.h"
#include "SoundData.h"
#include "Sound.h"

namespace ouzel
{
    namespace audio
    {
        Audio::Audio()
        {
        }

        Audio::~Audio()
        {
        }

        bool Audio::init()
        {
            return true;
        }

        SoundDataPtr Audio::createSoundData()
        {
            SoundDataPtr soundData(new SoundData());
            return soundData;
        }

        SoundPtr Audio::createSound()
        {
            SoundPtr sound(new Sound());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
