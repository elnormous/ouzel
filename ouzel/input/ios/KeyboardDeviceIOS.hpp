// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceIOS: public InputDevice
        {
        public:
            KeyboardDeviceIOS(InputSystem& initInputSystem,
                              uint32_t initId):
                InputDevice(initInputSystem, initId)
            {
            }

            virtual ~KeyboardDeviceIOS() {}
        };
    } // namespace input
} // namespace ouzel
