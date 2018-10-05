// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDeviceMacOS: public MouseDevice
        {
        public:
            MouseDeviceMacOS(InputSystem& initInputSystem,
                             uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2& position);
            void setCursorLocked(bool locked);
        };
    } // namespace input
} // namespace ouzel
