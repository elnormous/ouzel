// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(Animator* animator, uint32_t initCount = 0);
            Repeat(const std::unique_ptr<Animator>& animator, uint32_t initCount = 0);

            virtual void reset() override;

        protected:
            virtual void updateProgress() override;

            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
