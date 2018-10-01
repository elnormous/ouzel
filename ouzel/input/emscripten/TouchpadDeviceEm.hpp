// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/TouchpadDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceEm: public TouchpadDevice
        {
        public:
            TouchpadDeviceEm(InputSystem& initInputSystem,
                             uint32_t initId):
                TouchpadDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceEm() {}
        };
    } // namespace input
} // namespace ouzel
