// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Fade: public Animator
        {
        public:
            Fade(float initLength, float initOpacity, bool initRelative = false);

            virtual void play() override;

        protected:
            virtual void updateProgress() override;

            float opacity;
            float startOpacity = 0.0f;
            float targetOpacity = 0.0f;
            float diff = 0.0f;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
