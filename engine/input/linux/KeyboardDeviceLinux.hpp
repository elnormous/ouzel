// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
#define OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP

#include "../KeyboardDevice.hpp"

namespace ouzel::input
{
    class KeyboardDeviceLinux final: public KeyboardDevice
    {
    public:
        KeyboardDeviceLinux(InputSystem& initInputSystem,
                            DeviceId initId):
            KeyboardDevice(initInputSystem, initId)
        {
        }
    };
}

#endif // OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
