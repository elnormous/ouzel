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
        for (std::shared_ptr<Node> node : _children)
        {
            node->_hasParent = false;
            node->_layer.reset();
        }
    }
    
    bool NodeContainer::addChild(std::shared_ptr<Node> const& node)
    {
        if (!hasChild(node) && !node->hasParent())
        {
            _children.push_back(node);
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool NodeContainer::removeChild(std::shared_ptr<Node> const& node)
    {
        std::vector<std::shared_ptr<Node>>::iterator i = std::find_if(_children.begin(), _children.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        if (i != _children.end())
        {
            node->removeFromLayer();
            node->_hasParent = false;
            node->_layer.reset();
            _children.erase(i);
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool NodeContainer::hasChild(std::shared_ptr<Node> const& node) const
    {
        std::vector<std::shared_ptr<Node>>::const_iterator i = std::find_if(_children.begin(), _children.end(), [node](std::shared_ptr<Node> const& p) {
            return p.get() == node.get();
        });
        
        return i != _children.end();
    }
}
