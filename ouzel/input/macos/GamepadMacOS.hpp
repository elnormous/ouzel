// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class GamepadMacOS: public InputDevice
        {
        public:
            GamepadMacOS(InputSystemMacOS& initInputSystemMacOS,
                         uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~GamepadMacOS() {}

        protected:
            InputSystemMacOS& inputSystemMacOS;
            std::string name;
        };
    }
}
