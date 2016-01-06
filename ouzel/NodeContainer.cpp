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
        if (!hasChild(node) && node->getParent().expired())
        {
            if (std::shared_ptr<Layer> layer = _layer.lock())
            {
                node->addToLayer(layer);
            }
            
            _children.push_back(node);
        }
    }
    
    void NodeContainer::removeChild(std::shared_ptr<Node> node)
    {
        std::vector<std::shared_ptr<Node>>::iterator i = std::find_if(_children.begin(), _children.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        if (i != _children.end())
        {
            if (!_layer.expired())
            {
                node->removeFromLayer();
            }
            
            node->_parent.reset();
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
