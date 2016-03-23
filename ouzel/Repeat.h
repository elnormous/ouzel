// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Animator.h"
#include "Types.h"

namespace ouzel
{
    namespace scene
    {
        class Repeat: public Animator
        {
        public:
            Repeat(const AnimatorPtr& animator, uint32_t count = 0);

            virtual void update(float delta) override;

            virtual void start(const NodePtr& node) override;
            virtual void reset() override;

        protected:
            AnimatorPtr _animator;
            uint32_t _count = 0;
            uint32_t _currentCount = 0;
        };
    } // namespace scene
} // namespace ouzel
