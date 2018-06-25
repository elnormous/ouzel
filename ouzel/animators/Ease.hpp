// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Ease final: public Animator
        {
        public:
            enum class Mode
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

            Ease(Animator* animator, Mode initModee, Func initFunc);
            Ease(const std::unique_ptr<Animator>& animator, Mode initMode, Func initFunc);

        protected:
            void updateProgress() override;

        private:
            Mode mode;
            Func func;
        };
    } // namespace scene
} // namespace ouzel
