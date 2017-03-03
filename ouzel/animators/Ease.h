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

            Ease(Animator* aAnimator, Type aType, Func aFunc);
            virtual ~Ease();

            virtual void play() override;
            virtual void reset() override;

        protected:
            virtual void removeAnimator(Animator* oldAnimator) override;
            virtual void updateProgress() override;

            Animator* animator = nullptr;
            Type type;
            Func func;
        };
    } // namespace scene
} // namespace ouzel
