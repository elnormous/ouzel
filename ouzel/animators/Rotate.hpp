// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_ROTATE_HPP
#define OUZEL_ROTATE_HPP

#include "animators/Animator.hpp"
#include "math/Quaternion.hpp"

namespace ouzel
{
    namespace scene
    {
        class Rotate final: public Animator
        {
        public:
            Rotate(float initLength, const Vector3<float>& initRotation, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> rotation;
            Vector3<float> startRotation;
            Vector3<float> targetRotation;
            Vector3<float> diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_ROTATE_HPP
