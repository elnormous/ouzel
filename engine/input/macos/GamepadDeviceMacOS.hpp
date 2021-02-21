// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP
#define OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP

#include <string>
#include "../GamepadDevice.hpp"

namespace ouzel::input::macos
{
    class InputSystem;

    class GamepadDevice: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem, DeviceId initId);

        virtual void setPlayerIndex(std::int32_t) {}

    protected:
        std::string name;
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEMACOS_HPP
