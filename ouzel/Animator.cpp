// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Animator.h"
#include "Utils.h"

namespace ouzel
{
    Animator::Animator(float length):
        _length(length)
    {
        
    }
    
    void Animator::update(float delta)
    {
        if (_running)
        {
            _currentTime += delta;
            
            if (_currentTime >= _length)
            {
                _done = true;
                _running = false;
                _currentTime = _length;
                _progress = 1.0f;
            }
            else
            {
                _progress = _currentTime / _length;
            }
        }
    }
    
    void Animator::start(NodePtr const& node)
    {
        _running = true;
        _node = node;
    }
    
    void Animator::reset()
    {
        _done = false;
        _currentTime = 0.0f;
        _progress = 0.0f;
    }
}
