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

        protected:
            virtual void updateProgress() override;
        };
    } // namespace scene
} // namespace ouzel
