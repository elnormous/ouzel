// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
#define OUZEL_INPUT_GAMEPADDEVICEWIN_HPP

#include <cstdint>
#include "input/GamepadDevice.hpp"

namespace ouzel
{
    namespace input
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
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEWIN_HPP
