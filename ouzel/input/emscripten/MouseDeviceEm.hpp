// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemEm;

        class MouseDeviceEm: public InputDevice
        {
        public:
            MouseDeviceEm(InputSystemEm& initInputSystemEm,
                          uint32_t initId):
                InputDevice(initId),
                inputSystemEm(initInputSystemEm)
            {
            }

            virtual ~MouseDeviceEm() {}

        private:
            InputSystemEm& inputSystemEm;
        };
    } // namespace input
} // namespace ouzel
