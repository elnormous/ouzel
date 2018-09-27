// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class MouseAndroid: public InputDevice
        {
        public:
            MouseAndroid(InputSystemAndroid& initInputSystemAndroid,
                         uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemAndroid(initInputSystemAndroid)
            {
            }

            virtual ~MouseAndroid() {}

        private:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
