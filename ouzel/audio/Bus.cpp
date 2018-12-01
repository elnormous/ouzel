// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Bus.hpp"

namespace ouzel
{
    namespace audio
    {
        Bus::Bus(Audio& initAudio):
            audio(initAudio),
            objectId(0)
        {
        }

        void Bus::addFilter(Filter* filter)
        {
        }

        void Bus::removeFilter(Filter* filter)
        {
        }

        void Bus::addListener(Listener* listener)
        {
        }

        void Bus::removeListener(Listener* listener)
        {
        }
    } // namespace audio
} // namespace ouzel
