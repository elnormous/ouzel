// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    namespace scene
    {
        class Parallel: public Animator
        {
        public:
            Parallel(const std::vector<AnimatorPtr>& pAnimators);

            virtual void start(const NodePtr& targetNode) override;
            virtual void reset() override;

            virtual void setProgress(float progress) override;

        protected:
            std::vector<AnimatorPtr> animators;
        };
    } // namespace scene
} // namespace ouzel
