// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"
#include "utils/Types.h"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(const AnimatorPtr& aAnimator, uint32_t aCount = 0);

            virtual void start(Node* targetNode) override;
            virtual void reset() override;

        protected:
            virtual void updateProgress() override;

            AnimatorPtr animator;
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
