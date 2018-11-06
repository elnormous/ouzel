// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef GAMEPADDEVICEANDROID_HPP
#define GAMEPADDEVICEANDROID_HPP

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceAndroid final: public GamepadDevice
        {
        public:
            GamepadDeviceAndroid(InputSystem& initInputSystem,
                                 uint32_t initId);
        };
    } // namespace input
} // namespace ouzel

#endif // GAMEPADDEVICEANDROID_HPP
