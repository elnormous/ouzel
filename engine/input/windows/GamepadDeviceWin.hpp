// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
#define OUZEL_INPUT_GAMEPADDEVICEWIN_HPP

#include <cstdint>
#include "../GamepadDevice.hpp"

namespace ouzel::input::windows
{
    class InputSystem;

    class GamepadDevice : public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId);
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
