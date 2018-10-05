// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceAndroid: public GamepadDevice
        {
        public:
            GamepadDeviceAndroid(InputSystem& initInputSystem,
                                 uint32_t initId);
        };
    } // namespace input
} // namespace ouzel
