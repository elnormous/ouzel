// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
                             uint32_t initId):
                GamepadDevice(initInputSystem, initId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~GamepadDeviceWin() {}
        };
    } // namespace input
} // namespace ouzel
