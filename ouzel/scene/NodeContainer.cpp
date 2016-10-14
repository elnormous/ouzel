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
            for (const NodePtr& node : children)
            {
                if (entered) node->leave();
                node->setParent(nullptr);
            }
        }

        bool NodeContainer::addChild(const NodePtr& node)
        {
            if (!hasChild(node) && !node->hasParent())
            {
                node->setParent(this);
                if (entered) node->enter();
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
                if (entered) node->leave();
                node->setParent(nullptr);
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
                if (entered) node->leave();
                node->setParent(nullptr);
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

        void NodeContainer::enter()
        {
            entered = true;

            for (const NodePtr& node : children)
            {
                node->enter();
            }
        }

        void NodeContainer::leave()
        {
            entered = false;

            for (const NodePtr& node : children)
            {
                node->leave();
            }
        }
    } // namespace scene
} // namespace ouzel
