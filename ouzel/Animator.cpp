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
            if (_currentTime + delta >= _length)
            {
                _done = true;
                _running = false;
                setProgress(1.0f);
            }
            else
            {
                setProgress((_currentTime + delta) / _length);
            }
        }
    }
    
    void Animator::start(const NodePtr& node)
    {
        _running = true;
        _node = node;
    }
    
    void Animator::resume()
    {
        _running = true;
    }
    
    void Animator::pause()
    {
        _running = false;
    }
    
    void Animator::reset()
    {
        _done = false;
        _currentTime = 0.0f;
        setProgress(0.0f);
    }
    
    void Animator::setProgress(float progress)
    {
        _progress = progress;
        _currentTime = _progress * _length;
    }
}
