// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace scene
    {
        class Node;
        class Layer;

        class NodeContainer: public Noncopyable
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            void addChild(Node* node)
            {
                addChildNode(node);
            }

            template<class T> void addChild(const std::unique_ptr<T>& node)
            {
                addChildNode(node.get());
            }

            template<class T> void addChild(std::unique_ptr<T>&& node)
            {
                addChildNode(node.get());
                ownedChildren.push_back(std::move(node));
            }

            template<class T> bool removeChild(const std::unique_ptr<T>& node)
            {
                return removeChildNode(node.get());
            }

            bool removeChild(Node* node)
            {
                return removeChildNode(node);
            }

            bool moveChildToBack(Node* node);
            bool moveChildToFront(Node* node);
            virtual void removeAllChildren();
            virtual bool hasChild(Node* node, bool recursive = false) const;
            virtual const std::vector<Node*>& getChildren() const { return children; }

            Layer* getLayer() const { return layer; }

        protected:
            virtual void addChildNode(Node* node);
            virtual bool removeChildNode(Node* node);

            virtual void setLayer(Layer* newLayer);
            void findNodes(const Vector2& position, std::vector<std::pair<Node*, ouzel::Vector3>>& nodes) const;
            void findNodes(const std::vector<Vector2>& edges, std::vector<Node*>& nodes) const;

            virtual void enter();
            virtual void leave();

            std::vector<Node*> children;
            std::vector<std::unique_ptr<Node>> ownedChildren;

            Layer* layer = nullptr;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
