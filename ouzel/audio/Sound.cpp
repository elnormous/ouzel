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

        bool Sound::init(const SoundDataPtr& newSoundData)
        {
            soundData = newSoundData;

            return true;
        }

        void Sound::play(bool repeatSound)
        {
            repeat = repeatSound;
        }

        void Sound::stop(bool resetSound)
        {
            if (resetSound)
            {
                reset();
            }
        }

        void Sound::reset()
        {
        }
    } // namespace audio
} // namespace ouzel
