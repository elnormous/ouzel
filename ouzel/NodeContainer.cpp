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
                node->parent.reset();
                node->layer.reset();
            }
        }

        bool NodeContainer::addChild(const NodePtr& node)
        {
            if (!hasChild(node) && !node->hasParent())
            {
                node->parent = shared_from_this();
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
            std::vector<NodePtr>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                node->removeFromLayer();
                node->parent.reset();
                node->layer.reset();
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
                node->removeFromLayer();
                node->parent.reset();
                node->layer.reset();
            }

            children.clear();
        }

        bool NodeContainer::hasChild(const NodePtr& node, bool recursive) const
        {
            for (std::vector<NodePtr>::const_iterator i = children.begin(); i != children.end(); ++i)
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
