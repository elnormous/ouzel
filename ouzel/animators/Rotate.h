// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Rotate: public Animator
        {
        public:
            Rotate(float aLength, float aRotation, bool aRelative = false);

            virtual void start(Node* node) override;

        protected:
            virtual void updateProgress() override;

            float rotation;
            float startRotation = 0.0f;
            float targetRotation = 0.0f;
            float diff = 0.0f;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
