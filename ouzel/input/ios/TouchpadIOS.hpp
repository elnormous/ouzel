// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemIOS;

        class TouchpadIOS: public InputDevice
        {
        public:
            TouchpadIOS(InputSystemIOS& initInputSystemIOS,
                        uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemIOS(initInputSystemIOS)
            {
            }

            virtual ~TouchpadIOS() {}

        private:
            InputSystemIOS& inputSystemIOS;
        };
    } // namespace input
} // namespace ouzel
