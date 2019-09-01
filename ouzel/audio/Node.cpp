// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Node.hpp"
#include "Audio.hpp"

namespace ouzel
{
    namespace audio
    {
        void Node::addChild(Node& child)
        {
            if (child.parent != this)
            {
                if (child.parent)
                    child.parent->removeChild(child);

                auto i = std::find(children.begin(), children.end(), &child);
                if (i == children.end())
                {
                    child.parent = this;
                    children.push_back(&child);
                }
            }
        }

        void Node::removeChild(Node& child)
        {
            if (child.parent == this)
            {
                auto i = std::find(children.begin(), children.end(), &child);
                if (i != children.end())
                {
                    child.parent = this;
                    children.erase(i);
                }
            }
        }
    } // namespace audio
} // namespace ouzel
