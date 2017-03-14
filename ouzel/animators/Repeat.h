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
            Repeat(uint32_t aCount = 0);

            virtual void reset() override;

        protected:
            virtual void addAnimator(Animator* animator) override;
            virtual bool removeAnimator(Animator* animator) override;

            virtual void updateProgress() override;

            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
