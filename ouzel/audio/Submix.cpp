// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Submix.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        Submix::Submix(Audio& initAudio):
            audio(initAudio),
            nodeId(0)
        {

        }

        void Submix::setOutput(Submix* newOutput)
        {
            output = newOutput;
        }

        void Submix::addFilter(Filter* filter)
        {

        }
    } // namespace audio
} // namespace ouzel
