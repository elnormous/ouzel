// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <numeric>
#include "Sequence.h"

namespace ouzel
{
    Sequence::Sequence(std::vector<AnimatorPtr> const& animators):
        Animator(std::accumulate(animators.begin(), animators.end(), 0.0f, [](float a, AnimatorPtr const& b) { return a + b->getLength(); })), _animators(animators)
    {
        
    }
    
    void Sequence::start(NodePtr const& node)
    {
        Animator::start(node);
        
        for (auto& animator : _animators)
        {
            animator->start(node);
        }
    }
    
    void Sequence::reset()
    {
        Animator::reset();
        
        for (auto& animator : _animators)
        {
            animator->reset();
        }
    }
    
    void Sequence::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        float time = 0.0f;
        
        for (auto& animator : _animators)
        {
            float animationLength = animator->getLength();
            
            if (!animationLength || _currentTime > time + animationLength)
            {
                animator->setProgress(1.0f);
            }
            else
            {
                animator->setProgress((_currentTime - time) / animationLength);
                break;
            }
            
            time += animator->getLength();
        }
    }
}
