// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Parallel.h"

namespace ouzel
{
    Parallel::Parallel(const std::vector<AnimatorPtr>& animators):
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
    
    void Parallel::start(const NodePtr& node)
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
    
    void Parallel::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        for (auto& animator : _animators)
        {
            float animationLength = animator->getLength();
            
            if (!animationLength || _currentTime > animationLength)
            {
                animator->setProgress(1.0f);
            }
            else
            {
                animator->setProgress(_currentTime / animationLength);
            }
        }
    }
}
