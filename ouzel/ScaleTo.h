// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
    class ScaleTo: public Animator
    {
    public:
        ScaleTo(float length, Vector2 const& scale);
        virtual void update(float delta) override;
        
        virtual void start(NodePtr const& node) override;
        
    protected:
        Vector2 _startScale;
        Vector2 _targetScale;
        Vector2 _diff;
    };
}
