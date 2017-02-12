// Copyright (C) 2017 Elviss Strazdins
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

        bool Sound::init(const SoundDataPtr& newSoundData)
        {
            soundData = newSoundData;
            ready = true;

            return true;
        }

        void Sound::setPosition(const Vector3& newPosition)
        {
            position = newPosition;
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
