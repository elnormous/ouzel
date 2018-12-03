// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Submix.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Submix::Submix(Audio& initAudio):
            Mix(initAudio)
        {

        }

        void Submix::setOutput(Mix* newOutput)
        {
            output = newOutput;
        }
    } // namespace audio
} // namespace ouzel
