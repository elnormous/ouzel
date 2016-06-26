// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioAL.h"
#include "SoundDataAL.h"
#include "SoundAL.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        AudioAL::AudioAL():
            Audio(Driver::OPENAL)
        {
        }

        AudioAL::~AudioAL()
        {

        }

        void AudioAL::free()
        {
            Audio::free();
        }

        bool AudioAL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

            ready = true;
            
            return true;
        }

        SoundDataPtr AudioAL::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataAL());
            return soundData;
        }

        SoundPtr AudioAL::createSound()
        {
            SoundPtr sound(new SoundAL());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
