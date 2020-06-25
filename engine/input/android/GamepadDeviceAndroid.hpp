// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
#define OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input
{
    class GamepadDeviceAndroid final: public GamepadDevice
    {
    public:
        GamepadDeviceAndroid(InputSystem& initInputSystem,
                                DeviceId initId);
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
