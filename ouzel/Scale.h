// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Scale: public Animator
        {
        public:
            Scale(float pLength, const Vector2& pScale, bool pRelative);

            virtual void start(const NodePtr& targetNode) override;

            virtual void setProgress(float progress) override;

        protected:
            Vector2 scale;
            Vector2 startScale;
            Vector2 targetScale;
            Vector2 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
