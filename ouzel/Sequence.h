// Copyright (C) 2015 Elviss Strazdins
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
        Sequence(std::vector<AnimatorPtr> const& animators);
        
        virtual void update(float delta) override;
        
        virtual void start(NodePtr const& node) override;
        virtual void reset() override;
        
    protected:
        std::vector<AnimatorPtr> _animators;
        std::vector<AnimatorPtr>::iterator _current;
    };
}
