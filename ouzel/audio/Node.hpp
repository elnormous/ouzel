// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <cstdint>
#include <algorithm>
#include <vector>
#include "audio/Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        class Node
        {
        public:
            Node(Audio& initAudio):
                audio(initAudio)
            {
            }

            virtual ~Node()
            {
                if (parent)
                    parent->removeChild(*this);
            }

            Node(const Node&) = delete;
            Node& operator=(const Node&) = delete;

            Node(Node&&) = delete;
            Node& operator=(Node&&) = delete;

            void addChild(Node& child)
            {
                auto i = std::find(children.begin(), children.end(), &child);
                if (i == children.end())
                {
                    child.parent = this;
                    children.push_back(&child);
                    audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::AddChildCommand(objectId, child.objectId)));
                }
            }

            void removeChild(Node& child)
            {
                auto i = std::find(children.begin(), children.end(), &child);
                if (i != children.end())
                {
                    child.parent = this;
                    children.erase(i);
                    audio.addCommand(std::unique_ptr<mixer::Command>(new mixer::RemoveChildCommand(objectId, child.objectId)));
                }
            }

        private:
            Audio& audio;
            uintptr_t objectId = 0;
            Node* parent = nullptr;
            std::vector<Node*> children;
        };
    }
}

#endif // OUZEL_AUDIO_NODE_HPP
