// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEEM_HPP
#define OUZEL_INPUT_MOUSEDEVICEEM_HPP

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDeviceEm final: public MouseDevice
        {
        public:
            MouseDeviceEm(InputSystem& initInputSystem,
                          DeviceId initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICEEM_HPP
