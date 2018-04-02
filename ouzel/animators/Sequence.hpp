// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Sequence: public Animator
        {
        public:
            explicit Sequence(const std::vector<Animator*>& initAnimators);
            explicit Sequence(const std::vector<std::unique_ptr<Animator>>& initAnimators);

            virtual void play() override;

        protected:
            virtual void updateProgress() override;

            Animator* currentAnimator = nullptr;
        };
    } // namespace scene
} // namespace ouzel
