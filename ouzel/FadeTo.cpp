// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "FadeTo.h"
#include "Node.h"

namespace ouzel
{
    FadeTo::FadeTo(float length, float rotation):
        Animator(length), _targetOpacity(rotation)
    {
        
    }
    
    void FadeTo::update(float delta)
    {
        Animator::update(delta);
        
        if (NodePtr node = _node.lock())
        {
            node->setOpacity(_startOpacity + (_diff * _progress));
        }
    }
    
    void FadeTo::start(NodePtr const& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startOpacity = node->getOpacity();
            _diff = _targetOpacity - _startOpacity;
        }
    }
}
