// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Fade: public Animator
        {
        public:
            Fade(float aLength, float aOpacity, bool aRelative = false);

            virtual void start(Node* targetNode) override;

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
