// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "AutoPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class Layer;
    class Node;
    
    class NodeContainer: public Noncopyable, public ReferenceCounted
    {
    public:
        NodeContainer();
        virtual ~NodeContainer();
        
        virtual void addChild(Node* node);
        virtual void removeChild(Node* node);
        virtual bool hasChild(Node* node) const;
        virtual const std::vector<AutoPtr<Node>>& getChildren() const { return _children; }
        
    protected:
        std::vector<AutoPtr<Node>> _children;
        
        Layer* _layer = nullptr;
    };
}
