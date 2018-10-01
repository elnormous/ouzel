// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceWin: public KeyboardDevice
        {
        public:
            KeyboardDeviceWin(InputSystem& initInputSystem,
                              uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            virtual ~KeyboardDeviceWin() {}
        };
    } // namespace input
} // namespace ouzel
