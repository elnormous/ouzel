// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class TouchpadWin: public InputDevice
        {
        public:
            TouchpadWin(InputSystemWin& initInputSystemWin,
                        uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemWin(initInputSystemWin)
            {
            }

            virtual ~TouchpadWin() {}

        private:
            InputSystemWin& inputSystemWin;
        };
    } // namespace input
} // namespace ouzel
