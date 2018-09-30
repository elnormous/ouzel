// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class KeyboardDeviceAndroid: public InputDevice
        {
        public:
            KeyboardDeviceAndroid(InputSystemAndroid& initInputSystemAndroid,
                                  uint32_t initId):
                InputDevice(initId),
                inputSystemAndroid(initInputSystemAndroid)
            {
            }

            virtual ~KeyboardDeviceAndroid() {}

        private:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
