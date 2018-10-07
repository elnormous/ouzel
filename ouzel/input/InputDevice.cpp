// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "InputDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        InputDevice::InputDevice(InputSystem& initInputSystem, uint32_t initId, Controller::Type initType):
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
