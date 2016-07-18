// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <list>
#include <memory>
#include "utils/Types.h"
#include "utils/Noncopyable.h"

namespace ouzel
{
    namespace scene
    {
        class Layer;
        class Node;

        class NodeContainer: public Noncopyable, public std::enable_shared_from_this<NodeContainer>
        {
        public:
            NodeContainer();
            virtual ~NodeContainer();

            virtual bool addChild(const NodePtr& node);
            virtual bool removeChild(const NodePtr& node);
            virtual void removeAllChildren();
            virtual bool hasChild(const NodePtr& node, bool recursive = false) const;
            virtual const std::list<NodePtr>& getChildren() const { return children; }

        protected:
            std::list<NodePtr> children;
        };
    } // namespace scene
} // namespace ouzel
