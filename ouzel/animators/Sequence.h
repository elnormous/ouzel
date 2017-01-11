// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "animators/Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Sequence: public Animator
        {
        public:
            Sequence(const std::vector<Animator*>& aAnimators);

            virtual void start(Node* newTargetNode) override;
            virtual void reset() override;

        protected:
            virtual void removeAnimator(Animator* animator) override;
            virtual void updateProgress() override;

            std::vector<Animator*> animators;
        };
    } // namespace scene
} // namespace ouzel
