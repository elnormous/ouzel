// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemIOS;

        class TouchpadDeviceIOS: public InputDevice
        {
        public:
            TouchpadDeviceIOS(InputSystemIOS& initInputSystemIOS,
                              uint32_t initId):
                InputDevice(initId),
                inputSystemIOS(initInputSystemIOS)
            {
            }

            virtual ~TouchpadDeviceIOS() {}

        private:
            InputSystemIOS& inputSystemIOS;
        };
    } // namespace input
} // namespace ouzel
