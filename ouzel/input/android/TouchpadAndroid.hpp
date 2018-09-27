// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace input
    {
        class InputSystemAndroid;

        class TouchpadAndroid: public InputDevice
        {
        public:
            TouchpadAndroid(InputSystemAndroid& initInputSystemAndroid,
                            uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemAndroid(initInputSystemAndroid)
            {
            }

            virtual ~TouchpadAndroid() {}

        private:
            InputSystemAndroid& inputSystemAndroid;
        };
    } // namespace input
} // namespace ouzel
