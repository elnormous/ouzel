// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    class Repeat: public Animator
    {
    public:
        Repeat(AnimatorPtr const& animator, uint32_t count = 0);
        
        virtual void update(float delta) override;
        
        virtual void start(NodePtr const& node) override;
        virtual void reset() override;
        
    protected:
        AnimatorPtr _animator;
        uint32_t _count = 0;
        uint32_t _currentCount = 0;
    };
}
