// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SCALE_HPP
#define OUZEL_SCALE_HPP

#include "animators/Animator.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace scene
    {
        class Scale final: public Animator
        {
        public:
            Scale(float initLength, const Vector3<float>& initScale, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            Vector3<float> scale;
            Vector3<float> startScale;
            Vector3<float> targetScale;
            Vector3<float> diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel

#endif // OUZEL_SCALE_HPP
