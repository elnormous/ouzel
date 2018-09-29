// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class GamepadDeviceAndroid final
        {
        public:
            GamepadDeviceAndroid(InputSystemAndroid& initInputSystemAndroid,
                                 uint32_t initDeviceId);
            virtual ~GamepadDeviceAndroid();

            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            InputSystemAndroid& inputSystemAndroid;
            uint32_t deviceId;
        };
    } // namespace input
} // namespace ouzel
