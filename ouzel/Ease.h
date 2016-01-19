// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    class Ease: public Animator
    {
    public:
        enum class Type
        {
            IN,
            OUT,
            INOUT
        };
        
        enum class Func
        {
            SIN,
            QUAD,
            CUBIC,
            QUART,
            QUINT,
            EXPO,
            CIRC,
            BACK,
            ELASTIC,
            BOUNCE
        };
        
        Ease(AnimatorPtr const& animator, Type type, Func func);
        
        virtual void update(float delta) override;
        
        virtual void start(NodePtr const& node) override;
        virtual void reset() override;
        
        virtual void setProgress(float progress) override;
        
    protected:
        AnimatorPtr _animator;
        Type _type;
        Func _func;
    };
}
