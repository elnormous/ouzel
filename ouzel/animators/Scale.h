// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"
#include "math/Vector3.h"

namespace ouzel
{
    namespace scene
    {
        class Scale: public Animator
        {
        public:
            Scale(float aLength, const Vector3& aScale, bool aRelative = false);

            virtual void start(Node* newTargetNode) override;

        protected:
            virtual void updateProgress() override;

            Vector3 scale;
            Vector3 startScale;
            Vector3 targetScale;
            Vector3 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
