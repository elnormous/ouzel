// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <cstdint>
#include <vector>

namespace ouzel
{
    namespace audio
    {
        class Node
        {
        public:
            Node() {}

        private:
            uintptr_t objectId = 0;
            Node* parent = nullptr;
            std::vector<Node*> children;
        };
    }
}

#endif // OUZEL_AUDIO_NODE_HPP
