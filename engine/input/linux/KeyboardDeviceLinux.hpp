// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
#define OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
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
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
