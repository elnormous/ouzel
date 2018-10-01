// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceMacOS: public KeyboardDevice
        {
        public:
            KeyboardDeviceMacOS(InputSystem& initInputSystem,
                                uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            virtual ~KeyboardDeviceMacOS() {}
        };
    } // namespace input
} // namespace ouzel
