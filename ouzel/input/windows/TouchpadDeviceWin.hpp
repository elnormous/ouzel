// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class TouchpadDeviceWin: public InputDevice
        {
        public:
            TouchpadDeviceWin(InputSystemWin& initInputSystemWin,
                              uint32_t initId):
                InputDevice(initId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~TouchpadDeviceWin() {}

        private:
            InputSystemWin& inputSystemWin;
        };
    } // namespace input
} // namespace ouzel
