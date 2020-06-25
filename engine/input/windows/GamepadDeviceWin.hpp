// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
#define OUZEL_INPUT_GAMEPADDEVICEWIN_HPP

#include <cstdint>
#include "../GamepadDevice.hpp"

namespace ouzel::input
{
    class GamepadDeviceWin: public GamepadDevice
    {
    public:
        GamepadDeviceWin(InputSystem& initInputSystem,
                            DeviceId initId):
            GamepadDevice(initInputSystem, initId)
        {
        }
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
