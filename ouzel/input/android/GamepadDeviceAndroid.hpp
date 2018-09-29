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
                                 uint32_t initDeviceId);
            virtual ~GamepadDeviceAndroid();

            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
