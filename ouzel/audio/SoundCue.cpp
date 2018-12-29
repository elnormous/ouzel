// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "SoundCue.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        SoundCue::SoundCue(Audio& initAudio):
            Sound(initAudio, 0)
        {
        }
    } // namespace audio
} // namespace ouzel
