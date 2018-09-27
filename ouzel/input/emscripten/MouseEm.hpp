// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemEm;

        class MouseEm: public InputDevice
        {
        public:
            MouseEm(InputSystemEm& initInputSystemEm,
                    uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemEm(initInputSystemEm)
            {
            }

            virtual ~MouseEm() {}

        private:
            InputSystemEm& inputSystemEm;
        };
    } // namespace input
} // namespace ouzel
