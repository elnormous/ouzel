// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
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
        
        virtual bool addChild(std::shared_ptr<Node> const& node);
        virtual bool removeChild(std::shared_ptr<Node> const& node);
        virtual bool hasChild(std::shared_ptr<Node> const& node) const;
        virtual const std::vector<std::shared_ptr<Node>>& getChildren() const { return _children; }
        
    protected:
        std::vector<std::shared_ptr<Node>> _children;
    };
}
