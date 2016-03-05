// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
namespace scene
{
    class Move: public Animator
    {
    public:
        Move(float length, const Vector2& position, bool relative = false);
        
        virtual void start(const NodePtr& node) override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        Vector2 _position;
        Vector2 _startPosition;
        Vector2 _targetPosition;
        Vector2 _diff;
        bool _relative;
    };
} // namespace scene
} // namespace ouzel
