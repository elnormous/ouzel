// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"

namespace ouzel
{
    class RotateTo: public Animator
    {
    public:
        RotateTo(float length, float rotation);
        
        virtual void start(NodePtr const& node) override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        float _startRotation = 0.0f;
        float _targetRotation = 0.0f;
        float _diff = 0.0f;
    };
}
