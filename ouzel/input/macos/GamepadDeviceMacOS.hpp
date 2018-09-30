// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class GamepadDeviceMacOS: public InputDevice
        {
        public:
            GamepadDeviceMacOS(InputSystemMacOS& initInputSystemMacOS,
                               uint32_t initId):
                InputDevice(initId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~GamepadDeviceMacOS() {}

        protected:
            InputSystemMacOS& inputSystemMacOS;
            std::string name;
        };
    } // namespace input
} // namespace ouzel
