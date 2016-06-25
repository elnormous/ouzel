// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Sound.h"

namespace ouzel
{
    namespace audio
    {
        Sound::Sound()
        {
        }

        Sound::~Sound()
        {
        }

        void Sound::free()
        {
            ready = false;
        }

        bool Sound::init(const SoundDataPtr& newSoundData)
        {
            free();

            soundData = newSoundData;
            ready = true;

            return true;
        }

        bool Sound::play(bool repeatSound)
        {
            repeat = repeatSound;

            stop(true);

            return true;
        }

        bool Sound::stop(bool resetSound)
        {
            if (resetSound)
            {
                reset();
            }

            return true;
        }

        bool Sound::reset()
        {
            return true;
        }
    } // namespace audio
} // namespace ouzel
