// Ouzel by Elviss Strazdins

#include "Node.hpp"
#include "Audio.hpp"

namespace ouzel::audio
{
    void Node::addChild(Node& child)
    {
        if (child.parent != this)
        {
            if (child.parent)
                child.parent->removeChild(child);

            if (std::find(children.begin(), children.end(), &child) == children.end())
            {
                child.parent = this;
                children.push_back(&child);
                audio.addCommand(std::make_unique<mixer::AddChildCommand>(objectId, child.objectId));
            }
        }
    }

    void Node::removeChild(Node& child)
    {
        if (child.parent == this)
            if (const auto i = std::find(children.begin(), children.end(), &child); i != children.end())
            {
                child.parent = this;
                children.erase(i);
                audio.addCommand(std::make_unique<mixer::RemoveChildCommand>(objectId, child.objectId));
            }
    }
}
