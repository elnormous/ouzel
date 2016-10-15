// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include "utils/Types.h"
#include "math/Vector2.h"
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace scene
    {
        class NodeContainer: public Noncopyable, public std::enable_shared_from_this<NodeContainer>
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual bool addChild(const NodePtr& node);
            virtual bool removeChild(const NodePtr& node);
            virtual void removeAllChildren();
            virtual bool hasChild(const NodePtr& node, bool recursive = false) const;
            virtual const std::vector<NodePtr>& getChildren() const { return children; }

            virtual void pickNodes(const Vector2& position, std::vector<NodePtr>& nodes) const;
            virtual void pickNodes(const std::vector<Vector2>& edges, std::vector<NodePtr>& nodes) const;

        protected:
            virtual void enter();
            virtual void leave();

            std::vector<NodePtr> children;
            bool entered = false;
        };
    } // namespace scene
} // namespace ouzel
