// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Fade final: public Animator
        {
        public:
            Fade(float initLength, float initOpacity, bool initRelative = false);

            void play() override;

        protected:
            void updateProgress() override;

        private:
            float opacity;
            float startOpacity = 0.0F;
            float targetOpacity = 0.0F;
            float diff = 0.0F;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
