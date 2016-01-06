// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
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
    
    void NodeContainer::addChild(std::shared_ptr<Node> node)
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
    
    void NodeContainer::removeChild(std::shared_ptr<Node> node)
    {
        std::vector<std::shared_ptr<Node>>::iterator i = std::find_if(_children.begin(), _children.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
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
    
    bool NodeContainer::hasChild(std::shared_ptr<Node> node) const
    {
        std::vector<std::shared_ptr<Node>>::const_iterator i = std::find_if(_children.begin(), _children.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        return i != _children.end();
    }
}
