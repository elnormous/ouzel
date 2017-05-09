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
                node->setLayer(nullptr);
            }
        }

        void NodeContainer::addChild(Node* node)
        {
            if (node)
            {
                if (node->parent)
                {
                    node->parent->removeChild(node);
                }

                node->parent = this;
                node->setLayer(layer);
                if (entered) node->enter();
                children.push_back(node);
            }
        }

        void NodeContainer::addChild(std::unique_ptr<Node>&& node)
        {
            addChild(node.get());
            ownedChildren.push_back(std::forward<std::unique_ptr<Node>>(node));
        }

        bool NodeContainer::removeChild(Node* node)
        {
            std::vector<std::unique_ptr<Node>>::iterator ownedIterator = std::find_if(ownedChildren.begin(), ownedChildren.end(), [node](const std::unique_ptr<Node>& other) {
                return other.get() == node;
            });

            if (ownedIterator != ownedChildren.end())
            {
                ownedChildren.erase(ownedIterator);
            }

            std::vector<Node*>::iterator childIterator = std::find(children.begin(), children.end(), node);

            if (childIterator != children.end())
            {
                if (entered) node->leave();
                node->parent = nullptr;
                node->setLayer(nullptr);
                children.erase(childIterator);

                return true;
            }

            return false;
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

        bool NodeContainer::hasChild(Node* node, bool recursive) const
        {
            for (std::vector<Node*>::const_iterator i = children.begin(); i != children.end(); ++i)
            {
                Node* child = *i;

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

            for (Node* node : children)
            {
                node->enter();
            }
        }

        void NodeContainer::leave()
        {
            entered = false;

            for (Node* node : children)
            {
                node->leave();
            }
        }

        void NodeContainer::setLayer(Layer* newLayer)
        {
            layer = newLayer;

            for (Node* node : children)
            {
                node->setLayer(layer);
            }
        }

        void NodeContainer::findNodes(const Vector2& position, std::vector<Node*>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                Node* node = *i;

                if (!node->isHidden())
                {
                    node->findNodes(position, nodes);

                    if (node->isPickable() && node->pointOn(position))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](Node* a, Node* b) {
                                                               return a->worldOrder < b->worldOrder;
                                                           });

                        nodes.insert(upperBound, node);
                    }
                }
            }
        }

        void NodeContainer::findNodes(const std::vector<Vector2>& edges, std::vector<Node*>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                Node* node = *i;

                if (!node->isHidden())
                {
                    node->findNodes(edges, nodes);

                    if (node->isPickable() && node->shapeOverlaps(edges))
                    {
                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), node,
                                                           [](Node* a, Node* b) {
                                                               return a->worldOrder < b->worldOrder;
                                                           });

                        nodes.insert(upperBound, node);
                    }
                }
            }
        }
    } // namespace scene
} // namespace ouzel
