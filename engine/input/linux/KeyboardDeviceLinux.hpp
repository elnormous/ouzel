// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
#define OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP

#include "../KeyboardDevice.hpp"

namespace ouzel::input::linux
{
    class InputSystem;

    class KeyboardDevice final: public input::KeyboardDevice
    {
    public:
        KeyboardDevice(InputSystem& initInputSystem,
                       DeviceId initId);
    };
}

#endif // OUZEL_INPUT_KEYBOARDDEVICELINUX_HPP
