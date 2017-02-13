// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "AudioEmpty.h"
#include "SoundEmpty.h"

namespace ouzel
{
    namespace audio
    {
        AudioEmpty::AudioEmpty():
            Audio(Driver::EMPTY)
        {
        }

        SoundPtr AudioEmpty::createSound()
        {
            SoundPtr sound = std::make_shared<SoundEmpty>();
            return sound;
        }
    } // namespace audio
} // namespace ouzel
