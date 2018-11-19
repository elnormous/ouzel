// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

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
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_NODE_HPP
