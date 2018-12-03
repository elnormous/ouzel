// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GroupBus.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        GroupBus::GroupBus()
        {

        }

        void GroupBus::setOutput(Bus* newOutput)
        {
            output = newOutput;
        }
    } // namespace audio
} // namespace ouzel
