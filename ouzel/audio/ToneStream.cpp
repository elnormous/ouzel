// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "ToneStream.hpp"

namespace ouzel
{
    namespace audio
    {
        void ToneStream::reset()
        {
            Stream::reset();

            offset = 0;
        }
    } // namespace audio
} // namespace ouzel
