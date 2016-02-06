// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Move.h"
#include "Node.h"

namespace ouzel
{
    Move::Move(float length, const Vector2& position, bool relative):
        Animator(length), _position(position), _relative(relative)
    {
        
    }
    
    void Move::start(const NodePtr& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startPosition = node->getPosition();
            _targetPosition = _relative ? _startPosition + _position : _position;
            
            _diff = _targetPosition - _startPosition;
        }
    }
    
    void Move::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        if (NodePtr node = _node.lock())
        {
            node->setPosition(_startPosition + (_diff * _progress));
        }
    }
}
