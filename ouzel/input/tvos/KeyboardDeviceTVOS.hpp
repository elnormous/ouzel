// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemTVOS;

        class KeyboardDeviceTVOS: public InputDevice
        {
        public:
            KeyboardDeviceTVOS(InputSystemTVOS& initInputSystemTVOS,
                               uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemTVOS(initInputSystemTVOS)
            {
            }

            virtual ~KeyboardDeviceTVOS() {}

        private:
            InputSystemTVOS& inputSystemTVOS;
        };
    } // namespace input
} // namespace ouzel
