// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Ease.h"

namespace ouzel
{
    Ease::Ease(AnimatorPtr const& animator, Type type, Func func):
        Animator(animator->getLength()), _animator(animator), _type(type), _func(func)
    {

    }
    
    void Ease::start(NodePtr const& node)
    {
        Animator::start(node);
        
        _animator->start(node);
    }
    
    void Ease::reset()
    {
        Animator::reset();
        
        _animator->reset();
    }
    
    void Ease::setProgress(float progress)
    {
        Animator::setProgress(progress);
        
        _animator->setProgress(progress);
    }
}
