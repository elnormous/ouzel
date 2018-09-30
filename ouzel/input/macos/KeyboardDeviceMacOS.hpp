// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"
#include "input/Keyboard.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class KeyboardDeviceMacOS: public KeyboardDevice
        {
        public:
            KeyboardDeviceMacOS(InputSystemMacOS& initInputSystemMacOS,
                                uint32_t initId):
                KeyboardDevice(initId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~KeyboardDeviceMacOS() {}

            void handleKeyPress(Keyboard::Key key, uint32_t modifiers);
            void handleKeyRelease(Keyboard::Key key, uint32_t modifiers);

        private:
            InputSystemMacOS& inputSystemMacOS;
        };
    } // namespace input
} // namespace ouzel
