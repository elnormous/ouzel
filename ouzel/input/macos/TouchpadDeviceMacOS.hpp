// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/TouchpadDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDeviceMacOS: public TouchpadDevice
        {
        public:
            TouchpadDeviceMacOS(InputSystem& initInputSystem,
                                uint32_t initId):
                TouchpadDevice(initInputSystem, initId)
            {
            }

            virtual ~TouchpadDeviceMacOS() {}
        };
    } // namespace input
} // namespace ouzel
