// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDeviceAndroid: public MouseDevice
        {
        public:
            MouseDeviceAndroid(InputSystem& initInputSystem,
                               uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            virtual ~MouseDeviceAndroid() {}
        };
    } // namespace input
} // namespace ouzel
