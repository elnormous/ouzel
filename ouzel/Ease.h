// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    namespace scene
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
                SINE,
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

            Ease(const AnimatorPtr& animator, Type type, Func func);

            virtual void start(const NodePtr& node) override;
            virtual void reset() override;

            virtual void setProgress(float progress) override;

        protected:
            AnimatorPtr _animator;
            Type _type;
            Func _func;
        };
    } // namespace scene
} // namespace ouzel
