// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "animators/Animator.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace scene
    {
        class Move final: public Animator
        {
        public:
            Move(float initLength, const Vector3& initPosition, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3 position;
            Vector3 startPosition;
            Vector3 targetPosition;
            Vector3 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
