// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Parallel.h"

namespace ouzel
{
    Parallel::Parallel(std::vector<AnimatorPtr> const& animators):
        Animator(0.0f), _animators(animators)
    {
        for (auto& animator : _animators)
        {
            if (animator->getLength() > _length)
            {
                _length = animator->getLength();
            }
        }
    }
    
    void Parallel::update(float delta)
    {
        Animator::update(delta);
        
        for (auto& animator : _animators)
        {
            animator->update(delta);
        }
    }
    
    void Parallel::start(NodePtr const& node)
    {
        Animator::start(node);
        
        for (auto& animator : _animators)
        {
            animator->start(node);
        }
    }
    
    void Parallel::reset()
    {
        Animator::reset();
        
        for (auto& animator : _animators)
        {
            animator->reset();
        }
    }
}
