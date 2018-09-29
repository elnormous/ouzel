// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemIOS;

        class KeyboardDeviceIOS: public InputDevice
        {
        public:
            KeyboardDeviceIOS(InputSystemIOS& initInputSystemIOS,
                              uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemIOS(initInputSystemIOS)
            {
            }

            virtual ~KeyboardDeviceIOS() {}

        private:
            InputSystemIOS& inputSystemIOS;
        };
    } // namespace input
} // namespace ouzel
