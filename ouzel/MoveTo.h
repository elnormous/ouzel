// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
    class MoveTo: public Animator
    {
    public:
        MoveTo(float length, const Vector2& position);
        
        virtual void start(const NodePtr& node) override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        Vector2 _startPosition;
        Vector2 _targetPosition;
        Vector2 _diff;
    };
}
