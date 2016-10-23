// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "animators/Animator.h"
#include "utils/Types.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Shake: public Animator
        {
        public:
            Shake(float aLength, const Vector2& aDistance, float aTimeScale);

            void start(Node* targetNode) override;

        protected:
            virtual void updateProgress() override;

            uint32_t seedX;
            uint32_t seedY;
            Vector2 distance;
            float timeScale;
            Vector2 startPosition;
        };
    } // namespace scene
} // namespace ouzel
