// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.hpp"
#include "Node.hpp"

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

        void NodeContainer::addChildNode(Node* node)
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

        bool NodeContainer::removeChildNode(Node* node)
        {
            bool result = false;

            std::vector<Node*>::iterator childIterator = std::find(children.begin(), children.end(), node);

            if (childIterator != children.end())
            {
                if (entered) node->leave();
                node->parent = nullptr;
                node->setLayer(nullptr);
                children.erase(childIterator);

                result = true;
            }

            std::vector<std::unique_ptr<Node>>::iterator ownedIterator = std::find_if(ownedChildren.begin(), ownedChildren.end(), [node](const std::unique_ptr<Node>& other) {
                return other.get() == node;
            });

            if (ownedIterator != ownedChildren.end())
            {
                ownedChildren.erase(ownedIterator);
            }

            return result;
        }

        bool NodeContainer::moveChildToBack(Node* node)
        {
            std::vector<Node*>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                std::rotate(children.begin(), i, i + 1);

                return true;
            }

            return false;
        }

        bool NodeContainer::moveChildToFront(Node* node)
        {
            std::vector<Node*>::iterator i = std::find(children.begin(), children.end(), node);

            if (i != children.end())
            {
                std::rotate(i, i + 1, children.end());

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
            ownedChildren.clear();
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

        void NodeContainer::findNodes(const Vector2& position, std::vector<std::pair<Node*, ouzel::Vector3>>& nodes) const
        {
            for (auto i = children.rbegin(); i != children.rend(); ++i)
            {
                Node* node = *i;

                if (!node->isHidden())
                {
                    node->findNodes(position, nodes);

                    if (node->isPickable() && node->pointOn(position))
                    {
                        std::pair<Node*, ouzel::Vector3> result = std::make_pair(node, node->convertWorldToLocal(position));

                        auto upperBound = std::upper_bound(nodes.begin(), nodes.end(), result,
                                                           [](const std::pair<Node*, Vector3>& a,
                                                              const std::pair<Node*, Vector3>& b) {
                                                               return a.first->worldOrder < b.first->worldOrder;
                                                           });

                        nodes.insert(upperBound, result);
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
