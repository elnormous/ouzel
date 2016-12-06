// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"
#include "math/Vector3.h"

namespace ouzel
{
    namespace scene
    {
        class Move: public Animator
        {
        public:
            Move(float aLength, const Vector3& aPosition, bool aRelative = false);

            virtual void start(Node* newTargetNode) override;

        protected:
            virtual void updateProgress() override;

            Vector3 position;
            Vector3 startPosition;
            Vector3 targetPosition;
            Vector3 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
