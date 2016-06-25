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

        void Audio::free()
        {
            ready = false;
        }

        bool Audio::init()
        {
            free();
            ready = true;

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
