// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Ease: public Animator
        {
        public:
            enum class Type
            {
                EASE_IN,
                EASE_OUT,
                EASE_INOUT
            };

            enum class Func
            {
                SINE,
                QUAD,
                CUBIC,
                QUART,
                QUINT,
                EXPO,
                CIRC,
                BACK,
                ELASTIC,
                BOUNCE
            };

            Ease(Animator* animator, Type initType, Func initFunc);
            Ease(const std::unique_ptr<Animator>& animator, Type initType, Func initFunc);

        protected:
            virtual void updateProgress() override;

            Type type;
            Func func;
        };
    } // namespace scene
} // namespace ouzel
