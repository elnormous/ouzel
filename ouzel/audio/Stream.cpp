// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Stream.hpp"

namespace ouzel
{
    namespace audio
    {
        Stream::Stream():
            playing(false), resetCount(0)
        {
        }

        Stream::~Stream()
        {
        }

        void Stream::reset()
        {
            ++resetCount;
            if (!repeating) playing = false;
        }
    } // namespace audio
} // namespace ouzel
