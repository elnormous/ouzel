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
        for (NodePtr node : _children)
        {
            node->_parent.reset();
            node->_layer.reset();
        }
    }
    
    bool NodeContainer::addChild(const NodePtr& node)
    {
        if (!hasChild(node) && !node->hasParent())
        {
            node->_parent = shared_from_this();
            _children.push_back(node);
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool NodeContainer::removeChild(const NodePtr& node)
    {
        std::vector<NodePtr>::iterator i = std::find_if(_children.begin(), _children.end(), [node](const NodePtr& p) {
            return p.get() == node.get();
        });
        
        if (i != _children.end())
        {
            node->removeFromLayer();
            node->_parent.reset();
            node->_layer.reset();
            _children.erase(i);
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void NodeContainer::removeAllChildren()
    {
        for (auto& node : _children)
        {
            node->removeFromLayer();
            node->_parent.reset();
            node->_layer.reset();
        }
        
        _children.clear();
    }
    
    bool NodeContainer::hasChild(const NodePtr& node) const
    {
        std::vector<NodePtr>::const_iterator i = std::find_if(_children.begin(), _children.end(), [node](const NodePtr& p) {
            return p.get() == node.get();
        });
        
        return i != _children.end();
    }
}
