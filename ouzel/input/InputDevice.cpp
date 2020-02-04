// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "InputDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        InputDevice::InputDevice(InputSystem& initInputSystem, uintptr_t initId, Controller::Type initType):
            inputSystem(initInputSystem), id(initId), type(initType)
        {
            inputSystem.addInputDevice(*this);
        }

        InputDevice::~InputDevice()
        {
            inputSystem.removeInputDevice(*this);
        }
    } // namespace input
} // namespace ouzel
