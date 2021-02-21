// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceWin.hpp"
#include "InputSystemWin.hpp"

namespace ouzel::input::windows
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice(initInputSystem, initId)
    {
    }
}
