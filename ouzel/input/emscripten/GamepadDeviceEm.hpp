// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm: public GamepadDevice
        {
        public:
            GamepadDeviceEm(InputSystem& initInputSystem,
                            uint32_t initId,
                            long initIndex);

            void update();

            long getIndex() const { return index; }

        private:
            void handleThumbAxisChange(double oldValue, double newValue,
                                       Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            long index = 0;
            double axis[64];
            double analogButton[64];
        };
    } // namespace input
} // namespace ouzel
