// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(const AnimatorPtr& pAnimator, uint32_t pCount = 0);

            virtual void update(float delta) override;

            virtual void start(const NodePtr& targetNode) override;
            virtual void reset() override;

        protected:
            AnimatorPtr animator;
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
