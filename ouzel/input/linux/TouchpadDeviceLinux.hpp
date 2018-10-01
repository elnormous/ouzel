// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/TouchpadDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceLinux: public TouchpadDevice
        {
        public:
            TouchpadDeviceLinux(InputSystem& initInputSystem,
                                uint32_t initId):
                TouchpadDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceLinux() {}
        };
    } // namespace input
} // namespace ouzel
