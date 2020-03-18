// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
#define OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceAndroid final: public GamepadDevice
        {
        public:
            GamepadDeviceAndroid(InputSystem& initInputSystem,
                                 DeviceId initId);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
