// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Repeat final: public Animator
        {
        public:
            explicit Repeat(Animator* animator, uint32_t initCount = 0);
            explicit Repeat(const std::unique_ptr<Animator>& animator, uint32_t initCount = 0);

            void reset() override;

        protected:
            void updateProgress() override;

        private:
            uint32_t count = 0;
            uint32_t currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
