// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Move: public Animator
        {
        public:
            Move(float aLength, const Vector2& aPosition, bool aRelative = false);

            virtual void start(Node* targetNode) override;

        protected:
            virtual void updateProgress() override;

            Vector2 position;
            Vector2 startPosition;
            Vector2 targetPosition;
            Vector2 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
