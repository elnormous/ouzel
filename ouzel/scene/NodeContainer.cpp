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

        void NodeContainer::addChild(const NodePtr& node)
        {
            NodeContainer* oldParent = node->parent;
            if (oldParent)
            {
                oldParent->removeChild(node);
            }

            node->setParent(this);
            if (entered) node->enter();
            children.push_back(node);
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

        void NodeContainer::findNodes(const Vector2& position, std::vector<NodePtr>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                const NodePtr& node = *i;

                if (!node->isHidden())
                {
                    if (node->isPickable() && node->pointOn(position))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](const NodePtr& a, const NodePtr& b) {
                                                               return a->getWorldZ() < b->getWorldZ();
                                                           });

                        nodes.insert(upperBound, node);
                    }

                    node->findNodes(position, nodes);
                }
            }
        }

        void NodeContainer::findNodes(const std::vector<Vector2>& edges, std::vector<NodePtr>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                const NodePtr& node = *i;

                if (!node->isHidden())
                {
                    if (node->isPickable() && node->shapeOverlaps(edges))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](const NodePtr& a, const NodePtr& b) {
                                                               return a->getWorldZ() < b->getWorldZ();
                                                           });

                        nodes.insert(upperBound, node);
                    }

                    node->findNodes(edges, nodes);
                }
            }
        }
    } // namespace scene
} // namespace ouzel
