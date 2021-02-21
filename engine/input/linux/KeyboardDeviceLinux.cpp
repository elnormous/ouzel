// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "../../core/Setup.h"
#include "KeyboardDeviceLinux.hpp"
#include "InputSystemLinux.hpp"

namespace ouzel::input::linux
{
    KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem,
                                   DeviceId initId):
        input::KeyboardDevice(initInputSystem, initId)
    {
    }
}