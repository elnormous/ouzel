// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "animators/Animator.h"
#include "math/Vector3.h"

namespace ouzel
{
    namespace scene
    {
        class Shake: public Animator
        {
        public:
            Shake(float aLength, const Vector3& aDistance, float aTimeScale);

            void start(Node* newTargetNode) override;

        protected:
            virtual void updateProgress() override;

            uint32_t seedX;
            uint32_t seedY;
            uint32_t seedZ;
            Vector3 distance;
            float timeScale;
            Vector3 startPosition;
        };
    } // namespace scene
} // namespace ouzel
