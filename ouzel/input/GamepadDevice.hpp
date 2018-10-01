// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDevice: public InputDevice
        {
        public:
            GamepadDevice(InputSystem& initInputSystem, uint32_t initId):
                InputDevice(initInputSystem, initId)
            {
            }

            void handleButtonValueChange(Gamepad::Button button, bool pressed, float value);
        };
    } // namespace input
} // namespace ouzel
