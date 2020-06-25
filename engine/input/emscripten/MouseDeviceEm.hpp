// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEEM_HPP
#define OUZEL_INPUT_MOUSEDEVICEEM_HPP

#include "../MouseDevice.hpp"

namespace ouzel::input
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
}

#endif // OUZEL_INPUT_MOUSEDEVICEEM_HPP
