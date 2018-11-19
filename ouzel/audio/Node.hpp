// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Node
        {
        public:
            Node() {}
            virtual ~Node() {}

            Node(const Node&) = delete;
            Node& operator=(const Node&) = delete;

            Node(Node&&) = delete;
            Node& operator=(Node&&) = delete;

            virtual void process(std::vector<float>& samples, uint32_t& channels,
                                 uint32_t& sampleRate, Vector3& position) = 0;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_NODE_HPP
