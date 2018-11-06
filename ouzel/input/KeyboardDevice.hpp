// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <future>
#include "input/InputDevice.hpp"
#include "input/Keyboard.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDevice: public InputDevice
        {
        public:
            KeyboardDevice(InputSystem& initInputSystem, uint32_t initId):
                InputDevice(initInputSystem, initId, Controller::Type::KEYBOARD)
            {
            }

            std::future<bool> handleKeyPress(Keyboard::Key key);
            std::future<bool> handleKeyRelease(Keyboard::Key key);
        };
    } // namespace input
} // namespace ouzel
