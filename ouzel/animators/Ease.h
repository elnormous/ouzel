// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"

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

            Ease(Type aType, Func aFunc);

        protected:
            virtual void addAnimator(Animator* animator) override;
            virtual bool removeAnimator(Animator* animator) override;
            
            virtual void updateProgress() override;

            Type type;
            Func func;
        };
    } // namespace scene
} // namespace ouzel
