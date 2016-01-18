// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RotateTo.h"
#include "Node.h"

namespace ouzel
{
    RotateTo::RotateTo(float length, float rotation):
        Animator(length), _targetRotation(rotation)
    {
        
    }
    
    void RotateTo::start(NodePtr const& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startRotation = node->getRotation();
            _diff = _targetRotation - _startRotation;
        }
    }
    
    void RotateTo::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        if (NodePtr node = _node.lock())
        {
            node->setRotation(_startRotation + (_diff * _progress));
        }
    }
}
