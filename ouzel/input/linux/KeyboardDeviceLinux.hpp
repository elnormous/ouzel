// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef KEYBOARDDEVICELINUX_HPP
#define KEYBOARDDEVICELINUX_HPP

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceLinux final: public KeyboardDevice
        {
        public:
            KeyboardDeviceLinux(InputSystem& initInputSystem,
                                uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }
        };
    } // namespace input
} // namespace ouzel

#endif // KEYBOARDDEVICELINUX_HPP
