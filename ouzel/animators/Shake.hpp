// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef SHAKE_HPP
#define SHAKE_HPP

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
            Shake(float initLength, const Vector3& initDistance, float initTimeScale);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            uint32_t seedX;
            uint32_t seedY;
            uint32_t seedZ;
            Vector3 distance;
            float timeScale;
            Vector3 startPosition;
        };
    } // namespace scene
} // namespace ouzel

#endif // SHAKE_HPP
