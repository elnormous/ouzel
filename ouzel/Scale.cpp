// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Scale.h"
#include "Node.h"

namespace ouzel
{
    Scale::Scale(float length, const Vector2& scale, bool relative):
        Animator(length), _scale(scale), _relative(relative)
    {
        
    }
    
    void Scale::start(const NodePtr& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startScale = node->getScale();
            _targetScale = _relative ? _startScale + _scale : _scale;
            
            _diff = _targetScale - _startScale;
        }
    }
    
    void Scale::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        if (NodePtr node = _node.lock())
        {
            node->setScale(_startScale + (_diff * _progress));
        }
    }
}
