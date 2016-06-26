// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioSL.h"
#include "SoundDataSL.h"
#include "SoundSL.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace audio
    {
        AudioSL::AudioSL():
            Audio(Driver::OPENSL)
        {
        }

        AudioSL::~AudioSL()
        {
        }

        void AudioSL::free()
        {
            Audio::free();
        }

        bool AudioSL::init()
        {
            if (!Audio::init())
            {
                return false;
            }

            free();

            ready = true;
            
            return true;
        }

        SoundDataPtr AudioSL::createSoundData()
        {
            SoundDataPtr soundData(new SoundDataSL());
            return soundData;
        }

        SoundPtr AudioSL::createSound()
        {
            SoundPtr sound(new SoundSL());
            return sound;
        }
    } // namespace audio
} // namespace ouzel
