// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class GamepadDeviceWin: public InputDevice
        {
        public:
            GamepadDeviceWin(InputSystemWin& initInputSystemWin,
                             uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~GamepadDeviceWin() {}

        protected:
            InputSystemWin& inputSystemWin;
        };
    } // namespace input
} // namespace ouzel
