// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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
                InputDevice(initInputSystem, initId)
            {
            }

            void handleKeyPress(Keyboard::Key key, uint32_t modifiers);
            void handleKeyRelease(Keyboard::Key key, uint32_t modifiers);
        };
    } // namespace input
} // namespace ouzel
