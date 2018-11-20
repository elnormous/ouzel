// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <cstdint>
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
            virtual ~Node();

            Node(const Node&) = delete;
            Node& operator=(const Node&) = delete;

            Node(Node&&) = delete;
            Node& operator=(Node&&) = delete;

            virtual void process(std::vector<float>& samples, uint16_t& channels,
                                 uint32_t& sampleRate, Vector3& position);

            void addOutputNode(Node* node);
            void removeOutputNode(Node* node);

        private:
            void addInputNode(Node* node);
            void removeInputNode(Node* node);

            std::vector<Node*> inputNodes;
            std::vector<Node*> outputNodes;

            std::vector<float> buffer;
        };
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_NODE_HPP
