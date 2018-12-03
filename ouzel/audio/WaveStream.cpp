// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "WaveStream.hpp"

namespace ouzel
{
    namespace audio
    {
        void WaveStream::reset()
        {
            Stream::reset();

            offset = 0;
        }
    } // namespace audio
} // namespace ouzel
