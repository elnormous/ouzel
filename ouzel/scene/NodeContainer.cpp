// Copyright (C) 2017 Elviss Strazdins
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
            for (auto& node : children)
            {
                if (entered) node->leave();
                node->parent = nullptr;
                node->layer = nullptr;
            }
        }

        void NodeContainer::addChild(const std::shared_ptr<Node>& node)
        {
            if (node)
            {
                if (node->parent)
                {
                    node->parent->removeChild(node);
                }

                node->parent = this;
                node->layer = layer;
                if (entered) node->enter();
                children.push_back(node);
            }
        }

        bool NodeContainer::removeChild(const std::shared_ptr<Node>& node)
        {
            std::vector<std::shared_ptr<Node>>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                if (entered) node->leave();
                node->parent = nullptr;
                node->layer = nullptr;
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
            for (auto& node : children)
            {
                if (entered) node->leave();
                node->parent = nullptr;
            }

            children.clear();
        }

        bool NodeContainer::hasChild(const std::shared_ptr<Node>& node, bool recursive) const
        {
            for (std::vector<std::shared_ptr<Node>>::const_iterator i = children.begin(); i != children.end(); ++i)
            {
                const std::shared_ptr<Node>& child = *i;

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

            for (const std::shared_ptr<Node>& node : children)
            {
                node->enter();
            }
        }

        void NodeContainer::leave()
        {
            entered = false;

            for (const std::shared_ptr<Node>& node : children)
            {
                node->leave();
            }
        }

        void NodeContainer::findNodes(const Vector2& position, std::vector<std::shared_ptr<Node>>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                const std::shared_ptr<Node>& node = *i;

                if (!node->isHidden())
                {
                    node->findNodes(position, nodes);

                    if (node->isPickable() && node->pointOn(position))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
                                                               return a->worldOrder < b->worldOrder;
                                                           });

                        nodes.insert(upperBound, node);
                    }
                }
            }
        }

        void NodeContainer::findNodes(const std::vector<Vector2>& edges, std::vector<std::shared_ptr<Node>>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                const std::shared_ptr<Node>& node = *i;

                if (!node->isHidden())
                {
                    node->findNodes(edges, nodes);

                    if (node->isPickable() && node->shapeOverlaps(edges))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
                                                               return a->worldOrder < b->worldOrder;
                                                           });

                        nodes.insert(upperBound, node);
                    }
                }
            }
        }
    } // namespace scene
} // namespace ouzel
