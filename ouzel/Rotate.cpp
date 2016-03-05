// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Rotate.h"
#include "Node.h"

namespace ouzel
{
namespace scene
{
    Rotate::Rotate(float length, float rotation, bool relative):
        Animator(length), _rotation(rotation), _relative(relative)
    {
        
    }
    
    void Rotate::start(const NodePtr& node)
    {
        Animator::start(node);
        
        if (node)
        {
            _startRotation = node->getRotation();
            _targetRotation = _relative ? _startRotation + _rotation : _rotation;
            
            _diff = _targetRotation - _startRotation;
        }
    }
    
    void Rotate::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        if (NodePtr node = _node.lock())
        {
            node->setRotation(_startRotation + (_diff * _progress));
        }
    }
} // namespace scene
} // namespace ouzel
