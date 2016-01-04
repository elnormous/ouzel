// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "NodeContainer.h"
#include "Node.h"

namespace ouzel
{
    NodeContainer::NodeContainer()
    {
        
    }
    
    NodeContainer::~NodeContainer()
    {
        
    }
    
    void NodeContainer::addChild(Node* node)
    {
        if (!hasChild(node) && node->getParent() == nullptr)
        {
            if (_layer)
            {
                node->addToLayer(_layer);
            }
            
            _children.push_back(node);
            node->_parent = this;
        }
    }
    
    void NodeContainer::removeChild(Node* node)
    {
        std::vector<AutoPtr<Node>>::iterator i = std::find(_children.begin(), _children.end(), node);
        
        if (i != _children.end())
        {
            if (_layer)
            {
                node->removeFromLayer();
            }
            
            node->_parent = nullptr;
            _children.erase(i);
        }
    }
    
    bool NodeContainer::hasChild(Node* node) const
    {
        std::vector<AutoPtr<Node>>::const_iterator i = std::find(_children.begin(), _children.end(), node);
        
        return i != _children.end();
    }
}
