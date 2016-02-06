// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
    class Scale: public Animator
    {
    public:
        Scale(float length, const Vector2& scale, bool relative);
        
        virtual void start(const NodePtr& node) override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        Vector2 _scale;
        Vector2 _startScale;
        Vector2 _targetScale;
        Vector2 _diff;
        bool _relative;
    };
}
