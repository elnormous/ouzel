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
        MoveTo(float length, Vector2 const& position);
        
        virtual void start(NodePtr const& node) override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        Vector2 _startPosition;
        Vector2 _targetPosition;
        Vector2 _diff;
    };
}
