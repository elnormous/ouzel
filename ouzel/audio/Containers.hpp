// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include "audio/Source.hpp"

namespace ouzel
{
    namespace audio
    {
        class ParallelContainer final: public Source
        {
        public:
        };

        class RandomContainer final: public Source
        {
        public:
        };

        class SequenceContainer final: public Source
        {
        public:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CONTAINERS_HPP
