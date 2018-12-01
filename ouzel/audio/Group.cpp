// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Group.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Group::Group(Audio& initAudio):
            Bus(initAudio)
        {

        }

        void Group::setOutput(Bus* newOutput)
        {
            output = newOutput;
        }
    } // namespace audio
} // namespace ouzel
