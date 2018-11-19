// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Pitch.hpp"

namespace ouzel
{
    namespace audio
    {
        Pitch::Pitch(Audio& initAudio):
            audio(initAudio)
        {
        }

        void Pitch::setPitch(float newPitch)
        {
            pitch = newPitch;
        }
    } // namespace audio
} // namespace ouzel
