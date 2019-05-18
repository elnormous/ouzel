// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_CONTAINERS_HPP
#define OUZEL_AUDIO_CONTAINERS_HPP

#include <algorithm>
#include <vector>
#include "audio/Node.hpp"

namespace ouzel
{
    namespace audio
    {
        class Container: public Node
        {
        public:
        };

        class Parallel final: public Container
        {
        public:
        };

        class Random final: public Container
        {
        public:
        };

        class Sequence final: public Container
        {
        public:
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_CONTAINERS_HPP
