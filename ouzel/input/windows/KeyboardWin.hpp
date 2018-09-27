// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class KeyboardWin: public InputDevice
        {
        public:
            KeyboardWin(InputSystemWin& initInputSystemWin,
                        uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~KeyboardWin() {}

        private:
            InputSystemWin& inputSystemWin;
        };
    } // namespace input
} // namespace ouzel
