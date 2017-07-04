// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioEmpty.h"
#include "SoundResourceEmpty.h"

namespace ouzel
{
    namespace audio
    {
        AudioEmpty::AudioEmpty():
            Audio(Driver::EMPTY)
        {
        }

        SoundResource* AudioEmpty::createSound()
        {
            SoundResource* sound = new SoundResourceEmpty();
            resources.push_back(std::unique_ptr<SoundResource>(sound));
            return sound;
        }
    } // namespace audio
} // namespace ouzel
