// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemIOS;

        class KeyboardIOS: public InputDevice
        {
        public:
            KeyboardIOS(InputSystemIOS& initInputSystemIOS,
                        uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemIOS(initInputSystemIOS)
            {
            }

            virtual ~KeyboardIOS() {}

        private:
            InputSystemIOS& inputSystemIOS;
        };
    } // namespace input
} // namespace ouzel
