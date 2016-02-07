// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    class Sequence: public Animator
    {
    public:
        Sequence(const std::vector<AnimatorPtr>& animators);
        
        virtual void start(const NodePtr& node) override;
        virtual void reset() override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        std::vector<AnimatorPtr> _animators;
    };
}
