// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "NodeContainer.h"
#include "Node.h"

namespace ouzel
{
namespace scene
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
        if (_locked)
        {
            _nodeAddList.insert(node);
            return false;
        }
        
        if (!hasChild(node) && !node->hasParent())
        {
            node->_remove = false;
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
        if (_locked)
        {
            node->_remove = true;
            _nodeRemoveList.insert(node);
            return false;
        }
        
        std::vector<NodePtr>::iterator i = std::find(_children.begin(), _children.end(), node);
        
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
        lock();
        
        for (auto& node : _children)
        {
            node->removeFromLayer();
            node->_parent.reset();
            node->_layer.reset();
        }
        
        _children.clear();
        
        unlock();
    }
    
    bool NodeContainer::hasChild(const NodePtr& node) const
    {
        std::vector<NodePtr>::const_iterator i = std::find(_children.begin(), _children.end(), node);
        
        return i != _children.end();
    }
    
    void NodeContainer::lock()
    {
        ++_locked;
    }
    
    void NodeContainer::unlock()
    {
        if (--_locked == 0)
        {
            if (!_nodeAddList.empty())
            {
                for (const NodePtr& node : _nodeAddList)
                {
                    addChild(node);
                }
                _nodeAddList.clear();
            }
            
            if (!_nodeRemoveList.empty())
            {
                for (const NodePtr& node : _nodeRemoveList)
                {
                    removeChild(node);
                }
                _nodeRemoveList.clear();
            }
        }
    }
} // namespace scene
} // namespace ouzel
