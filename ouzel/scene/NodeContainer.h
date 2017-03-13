// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <vector>
#include "utils/Noncopyable.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Node;

        class NodeContainer: public Noncopyable, public std::enable_shared_from_this<NodeContainer>
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual void addChild(const std::shared_ptr<Node>& node);
            virtual bool removeChild(const std::shared_ptr<Node>& node);
            virtual void removeAllChildren();
            virtual bool hasChild(const std::shared_ptr<Node>& node, bool recursive = false) const;
            virtual const std::vector<std::shared_ptr<Node>>& getChildren() const { return children; }

        protected:
            void findNodes(const Vector2& position, std::vector<std::shared_ptr<Node>>& nodes) const;
            void findNodes(const std::vector<Vector2>& edges, std::vector<std::shared_ptr<Node>>& nodes) const;

            virtual void enter();
            virtual void leave();

            std::vector<std::shared_ptr<Node>> children;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
