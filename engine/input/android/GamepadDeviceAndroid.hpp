// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
#define OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input::android
{
    class InputSystem;

    class GamepadDevice final: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId);
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEANDROID_HPP
