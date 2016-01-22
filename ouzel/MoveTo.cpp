// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MoveTo.h"
#include "Node.h"

namespace ouzel
{
    MoveTo::MoveTo(float length, const Vector2& position):
        Animator(length), _targetPosition(position)
    {
        
    }
    
    void MoveTo::start(const NodePtr& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startPosition = node->getPosition();
            _diff = _targetPosition - _startPosition;
        }
    }
    
    void MoveTo::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        if (NodePtr node = _node.lock())
        {
            node->setPosition(_startPosition + (_diff * _progress));
        }
    }
}
