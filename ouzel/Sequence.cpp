// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    Sequence::Sequence(std::vector<AnimatorPtr> const& animators):
        Animator(std::accumulate(animators.begin(), animators.end(), 0.0f, [](float a, AnimatorPtr const& b) { return a + b->getLength(); })), _animators(animators)
    {
        _current = _animators.begin();
    }
    
    void Sequence::update(float delta)
    {
        Animator::update(delta);
        
        while (true)
        {
            if (_current != _animators.end())
            {
                float remaining = (*_current)->getLength() - (*_current)->getCurrentTime();
                
                if (delta > remaining)
                {
                    (*_current)->update(remaining);
                    delta -= remaining;
                    _current++;
                }
                else
                {
                    (*_current)->update(delta);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    
    void Sequence::start(NodePtr const& node)
    {
        Animator::start(node);
        
        if (_current != _animators.end())
        {
            (*_current)->start(node);
        }
    }
    
    void Sequence::reset()
    {
        Animator::reset();
        
        for (auto& animator : _animators)
        {
            animator->reset();
        }
        
        _current = _animators.begin();
    }
}
