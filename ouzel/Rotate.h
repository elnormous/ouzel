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
            Rotate(float length, float rotation, bool relative);
            
            virtual void start(const NodePtr& node) override;
            
            virtual void setProgress(float progress) override;
            
        protected:
            float _rotation;
            float _startRotation = 0.0f;
            float _targetRotation = 0.0f;
            float _diff = 0.0f;
            bool _relative;
        };
    } // namespace scene
} // namespace ouzel
