// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceAndroid.hpp"
#include "InputSystemAndroid.hpp"

namespace ouzel::input::android
{
    GamepadDevice::GamepadDevice(InputSystem& initInputSystem,
                                 DeviceId initId):
        input::GamepadDevice(initInputSystem, initId)
    {
    }
}
