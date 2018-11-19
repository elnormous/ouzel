// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Delay.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Delay::Delay(Audio& initAudio):
            audio(initAudio)
        {
        }

        void Delay::setDelay(float newDelay)
        {
            delay = newDelay;
        }
    } // namespace audio
} // namespace ouzel
