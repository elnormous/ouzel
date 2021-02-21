// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "InputDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel::input
{
    InputDevice::InputDevice(InputSystem& initInputSystem, DeviceId initId, Controller::Type initType):
        inputSystem(initInputSystem), id(initId), type(initType)
    {
        inputSystem.addInputDevice(*this);
    }

    InputDevice::~InputDevice()
    {
        inputSystem.removeInputDevice(*this);
    }
}
