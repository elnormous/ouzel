// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <set>
#include <memory>
#include "Types.h"
#include "Noncopyable.h"

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
            virtual bool hasChild(const NodePtr& node) const;
            virtual const std::vector<NodePtr>& getChildren() const { return _children; }

        protected:
            void lock();
            void unlock();

            std::vector<NodePtr> _children;

            std::set<NodePtr> _nodeAddList;
            std::set<NodePtr> _nodeRemoveList;
            int32_t _locked = 0;
        };
    } // namespace scene
} // namespace ouzel
