// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceIOS: public InputDevice
        {
        public:
            TouchpadDeviceIOS(InputSystem& initInputSystem,
                              uint32_t initId):
                InputDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceIOS() {}
        };
    } // namespace input
} // namespace ouzel
