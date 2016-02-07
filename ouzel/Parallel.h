// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    class Parallel: public Animator
    {
    public:
        Parallel(const std::vector<AnimatorPtr>& animators);
        
        virtual void start(const NodePtr& node) override;
        virtual void reset() override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        std::vector<AnimatorPtr> _animators;
    };
}
