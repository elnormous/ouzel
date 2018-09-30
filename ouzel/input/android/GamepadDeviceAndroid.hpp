// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class GamepadDeviceAndroid: public InputDevice
        {
        public:
            GamepadDeviceAndroid(InputSystemAndroid& initInputSystemAndroid,
                                 uint32_t initId);
            virtual ~GamepadDeviceAndroid();

        protected:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
