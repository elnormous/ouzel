// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel::input::macos
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem, DeviceId initId):
        input::GamepadDevice(initInputSystem, initId)
    {
    }
}
