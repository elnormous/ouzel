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
    } // namespace audio
} // namespace ouzel
