// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP
#define OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP

#include <string>
#include "../GamepadDevice.hpp"

namespace ouzel::input
{
    class GamepadDeviceMacOS: public GamepadDevice
    {
    public:
        GamepadDeviceMacOS(InputSystem& initInputSystem,
                            DeviceId initId):
            GamepadDevice(initInputSystem, initId)
        {
        }

        virtual void setPlayerIndex(std::int32_t) {}

    protected:
        std::string name;
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP
