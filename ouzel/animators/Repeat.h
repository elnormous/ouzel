// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "animators/Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(const std::shared_ptr<Animator>& aAnimator, uint32_t aCount = 0);

            virtual void play() override;
            virtual void reset() override;

        protected:
            virtual void updateProgress() override;

            std::shared_ptr<Animator> animator;
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
