// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputEm;

        class GamepadEm: public Gamepad
        {
            friend InputEm;
        public:
            void update();

            long getIndex() const { return index; }

        protected:
            explicit GamepadEm(long initIndex);
            void handleThumbAxisChange(double oldValue, double newValue,
                                       GamepadButton negativeButton, GamepadButton positiveButton);

            long index = 0;
            double axis[64];
            double analogButton[64];
        };
    } // namespace input
} // namespace ouzel
