// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Gain.hpp"

namespace ouzel
{
    namespace audio
    {
        Gain::Gain()
        {
        }

        void Gain::setGain(float newGain)
        {
            gain = newGain;
        }
    } // namespace audio
} // namespace ouzel
