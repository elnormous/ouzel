// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include "Types.h"
#include "Noncopyable.h"

namespace ouzel
{
    class Layer;
    class Node;
    
    class NodeContainer: public Noncopyable
    {
    public:
        NodeContainer();
        virtual ~NodeContainer();
        
        virtual bool addChild(NodePtr const& node);
        virtual bool removeChild(NodePtr const& node);
        virtual bool hasChild(NodePtr const& node) const;
        virtual const std::vector<NodePtr>& getChildren() const { return _children; }
        
    protected:
        std::vector<NodePtr> _children;
    };
}
