// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.h"
#include "Node.h"

namespace ouzel
{
    namespace scene
    {
        NodeContainer::NodeContainer()
        {

        }

        NodeContainer::~NodeContainer()
        {
            for (NodePtr node : children)
            {
                node->addedToParent = false;
            }
        }

        bool NodeContainer::addChild(const NodePtr& node)
        {
            if (!hasChild(node) && !node->hasParent())
            {
                node->addedToParent = true;
                children.push_back(node);

                return true;
            }
            else
            {
                return false;
            }
        }

        bool NodeContainer::removeChild(const NodePtr& node)
        {
            std::list<NodePtr>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                node->addedToParent = false;
                children.erase(i);

                return true;
            }
            else
            {
                return false;
            }
        }

        void NodeContainer::removeAllChildren()
        {
            auto childrenCopy = children;

            for (auto& node : childrenCopy)
            {
                node->addedToParent = false;
            }

            children.clear();
        }

        bool NodeContainer::hasChild(const NodePtr& node, bool recursive) const
        {
            for (std::list<NodePtr>::const_iterator i = children.begin(); i != children.end(); ++i)
            {
                const NodePtr& child = *i;

                if (child == node || (recursive && child->hasChild(node, true)))
                {
                    return true;
                }
            }

            return false;
        }
    } // namespace scene
} // namespace ouzel
