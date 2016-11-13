// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "utils/Noncopyable.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Node;

        class NodeContainer: public Noncopyable
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual void addChild(Node* node);
            virtual bool removeChild(Node* node);
            virtual void removeAllChildren();
            virtual bool hasChild(Node* node, bool recursive = false) const;
            virtual const std::vector<Node*>& getChildren() const { return children; }

        protected:
            void findNodes(const Vector2& position, std::vector<Node*>& nodes) const;
            void findNodes(const std::vector<Vector2>& edges, std::vector<Node*>& nodes) const;

            virtual void enter();
            virtual void leave();

            std::vector<Node*> children;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
