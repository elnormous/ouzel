// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTDEVICE_HPP
#define OUZEL_INPUT_INPUTDEVICE_HPP

#include <cstdint>
#include "Controller.hpp"

namespace ouzel::input
{
    class InputSystem;

    class InputDevice
    {
    public:
        InputDevice(InputSystem& initInputSystem, DeviceId initId, Controller::Type initType);
        virtual ~InputDevice();

        InputDevice(const InputDevice&) = delete;
        InputDevice& operator=(const InputDevice&) = delete;
        InputDevice(InputDevice&&) = delete;
        InputDevice& operator=(InputDevice&&) = delete;

        auto getId() const noexcept { return id; }
        auto getType() const noexcept { return type; }

    protected:
        InputSystem& inputSystem;
        DeviceId id;
        Controller::Type type;
    };
}

#endif // OUZEL_INPUT_INPUTDEVICE_HPP
