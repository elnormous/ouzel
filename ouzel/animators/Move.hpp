// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MOVE_HPP
#define OUZEL_MOVE_HPP

#include "animators/Animator.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace scene
    {
        class Move final: public Animator
        {
        public:
            Move(float initLength, const Vector3<float>& initPosition, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> position;
            Vector3<float> startPosition;
            Vector3<float> targetPosition;
            Vector3<float> diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_MOVE_HPP
