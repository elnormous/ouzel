// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Move: public Animator
        {
        public:
            Move(float pLength, const Vector2& pPosition, bool pRelative = false);

            virtual void start(const NodePtr& targetNode) override;

        protected:
            virtual void updateProgress() override;

            Vector2 position;
            Vector2 startPosition;
            Vector2 targetPosition;
            Vector2 diff;
            bool relative;
        };
    } // namespace scene
} // namespace ouzel
