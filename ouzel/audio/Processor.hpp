// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_PROCESSOR_HPP
#define OUZEL_AUDIO_PROCESSOR_HPP

#include "audio/Object.hpp"

namespace ouzel
{
    namespace audio
    {
        class Processor: public Object
        {
        public:
            Processor()
            {
            }

            Processor(const Processor&) = delete;
            Processor& operator=(const Processor&) = delete;

            Processor(Processor&&) = delete;
            Processor& operator=(Processor&&) = delete;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_PROCESSOR_HPP
