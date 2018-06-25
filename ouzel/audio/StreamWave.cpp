// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "StreamWave.hpp"

namespace ouzel
{
    namespace audio
    {
        void StreamWave::reset()
        {
            Stream::reset();

            offset = 0;
        }
    } // namespace audio
} // namespace ouzel
