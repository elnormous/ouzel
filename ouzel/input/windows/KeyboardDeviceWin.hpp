// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class KeyboardDeviceWin: public InputDevice
        {
        public:
            KeyboardDeviceWin(InputSystemWin& initInputSystemWin,
                              uint32_t initId):
                InputDevice(initId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~KeyboardDeviceWin() {}

        private:
            InputSystemWin& inputSystemWin;
        };
    } // namespace input
} // namespace ouzel
