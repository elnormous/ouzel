// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemLinux;

        class KeyboardDeviceLinux: public InputDevice
        {
        public:
            KeyboardDeviceLinux(InputSystemLinux& initInputSystemLinux,
                                uint32_t initId):
                InputDevice(initId),
                inputSystemLinux(initInputSystemLinux)
            {
            }

            virtual ~KeyboardDeviceLinux() {}

        private:
            InputSystemLinux& inputSystemLinux;
        };
    } // namespace input
} // namespace ouzel
