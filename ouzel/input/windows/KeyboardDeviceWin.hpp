// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceWin: public KeyboardDevice
        {
        public:
            KeyboardDeviceWin(InputSystem& initInputSystem,
                              uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            void handleKeyPress(Keyboard::Key key, uint32_t modifiers);
            void update();

        private:
            bool leftShiftDown = false;
            bool rightShiftDown = false;
        };
    } // namespace input
} // namespace ouzel
