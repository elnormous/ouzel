// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Delay.hpp"

namespace ouzel
{
    namespace audio
    {
        Delay::Delay()
        {
        }

        void Delay::setDelay(float newDelay)
        {
            delay = newDelay;
        }
    } // namespace audio
} // namespace ouzel
