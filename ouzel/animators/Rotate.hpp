// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.hpp"
#include "math/Quaternion.hpp"

namespace ouzel
{
    namespace scene
    {
        class Rotate: public Animator
        {
        public:
            Rotate(float aLength, const Vector3& aRotation, bool aRelative = false);

            virtual void play() override;

        protected:
            virtual void updateProgress() override;

            Vector3 rotation;
            Vector3 startRotation;
            Vector3 targetRotation;
            Vector3 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
