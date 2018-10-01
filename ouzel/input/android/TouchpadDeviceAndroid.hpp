// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/TouchpadDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceAndroid: public TouchpadDevice
        {
        public:
            TouchpadDeviceAndroid(InputSystem& initInputSystem,
                                  uint32_t initId):
                TouchpadDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceAndroid() {}
        };
    } // namespace input
} // namespace ouzel
