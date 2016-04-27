// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"

namespace ouzel
{
    namespace scene
    {
        class Rotate: public Animator
        {
        public:
            Rotate(float pLength, float pRotation, bool pRelative);

            virtual void start(const NodePtr& node) override;

            virtual void setProgress(float progress) override;

        protected:
            float rotation;
            float startRotation = 0.0f;
            float targetRotation = 0.0f;
            float diff = 0.0f;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
