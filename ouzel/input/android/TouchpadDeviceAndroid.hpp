// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class TouchpadDeviceAndroid: public InputDevice
        {
        public:
            TouchpadDeviceAndroid(InputSystemAndroid& initInputSystemAndroid,
                                  uint32_t initId):
                InputDevice(initId),
                inputSystemAndroid(initInputSystemAndroid)
            {
            }

            virtual ~TouchpadDeviceAndroid() {}

        private:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
