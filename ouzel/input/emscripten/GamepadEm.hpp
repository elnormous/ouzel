// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadEm: public Gamepad
        {
        public:
            explicit GamepadEm(long initIndex);

            void update();

            long getIndex() const { return index; }

        private:
            void handleThumbAxisChange(double oldValue, double newValue,
                                       GamepadButton negativeButton, GamepadButton positiveButton);

            long index = 0;
            double axis[64];
            double analogButton[64];
        };
    } // namespace input
} // namespace ouzel
