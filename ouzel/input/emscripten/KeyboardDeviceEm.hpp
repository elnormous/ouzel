// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceEm: public KeyboardDevice
        {
        public:
            KeyboardDeviceEm(InputSystem& initInputSystem,
                             uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            virtual ~KeyboardDeviceEm() {}
        };
    } // namespace input
} // namespace ouzel
