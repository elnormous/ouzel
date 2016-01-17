// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MoveTo.h"
#include "Node.h"

namespace ouzel
{
    MoveTo::MoveTo(float length, Vector2 const& position):
        Animator(length), _targetPosition(position)
    {
        
    }
    
    void MoveTo::update(float delta)
    {
        Animator::update(delta);
        
        if (NodePtr node = _node.lock())
        {
            node->setPosition(_startPosition + (_offset * _progress));
        }
    }
    
    void MoveTo::start(NodePtr const& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startPosition = node->getPosition();
            _offset = _targetPosition - _startPosition;
        }
    }
}
