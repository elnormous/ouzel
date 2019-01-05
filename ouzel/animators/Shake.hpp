// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SHAKE_HPP
#define OUZEL_SHAKE_HPP

#include <cstdint>
#include <vector>
#include "animators/Animator.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace scene
    {
        class Shake final: public Animator
        {
        public:
            Shake(float initLength, const Vector3<float>& initDistance, float initTimeScale);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            uint32_t seedX;
            uint32_t seedY;
            uint32_t seedZ;
            Vector3<float> distance;
            float timeScale;
            Vector3<float> startPosition;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SHAKE_HPP
