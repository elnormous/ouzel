// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "animators/Animator.hpp"
#include "math/Quaternion.hpp"

namespace ouzel
{
    namespace scene
    {
        class Rotate final: public Animator
        {
        public:
            Rotate(float initLength, const Vector3& initRotation, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3 rotation;
            Vector3 startRotation;
            Vector3 targetRotation;
            Vector3 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
