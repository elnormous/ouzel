// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "animators/Animator.h"
#include "utils/Types.h"

namespace ouzel
{
    namespace scene
    {
        class Parallel: public Animator
        {
        public:
            Parallel(const std::vector<Animator*>& aAnimators);

            virtual void start(Node* newTargetNode) override;
            virtual void reset() override;

        protected:
            virtual void removeAnimator(Animator* animator) override;
            virtual void updateProgress() override;

            std::vector<Animator*> animators;
        };
    } // namespace scene
} // namespace ouzel
