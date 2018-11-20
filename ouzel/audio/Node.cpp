// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Node.hpp"

namespace ouzel
{
    namespace audio
    {
        Node::~Node()
        {
            for (Node* outputNode : outputNodes)
                outputNode->removeInputNode(this);
        }

        void Node::process(std::vector<float>& samples, uint16_t& channels,
                           uint32_t& sampleRate, Vector3& position)
        {
            buffer.resize(samples.size());
            std::fill(buffer.begin(), buffer.end(), 0.0F);

            for (Node* inputNode : inputNodes)
            {
                inputNode->process(buffer, channels, sampleRate, position);

                for (uint32_t i = 0; i < samples.size(); ++i)
                    samples[i] += buffer[i];
            }
        }

        void Node::addOutputNode(Node* node)
        {
            auto i = std::find(outputNodes.begin(), outputNodes.end(), node);
            if (i == outputNodes.end())
            {
                node->addInputNode(this);
                outputNodes.push_back(node);
            }
        }

        void Node::removeOutputNode(Node* node)
        {
            auto i = std::find(outputNodes.begin(), outputNodes.end(), node);
            if (i != outputNodes.end())
            {
                node->removeInputNode(this);
                outputNodes.erase(i);
            }
        }

        void Node::addInputNode(Node* node)
        {
            auto i = std::find(inputNodes.begin(), inputNodes.end(), node);
            if (i == inputNodes.end())
                inputNodes.push_back(node);
        }

        void Node::removeInputNode(Node* node)
        {
            auto i = std::find(inputNodes.begin(), inputNodes.end(), node);
            if (i != inputNodes.end())
                inputNodes.erase(i);
        }
    } // namespace audio
} // namespace ouzel
