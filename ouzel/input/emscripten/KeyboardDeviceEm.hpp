// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemEm;

        class KeyboardDeviceEm: public InputDevice
        {
        public:
            KeyboardDeviceEm(InputSystemEm& initInputSystemEm,
                             uint32_t initId):
                InputDevice(initId),
                inputSystemEm(initInputSystemEm)
            {
            }

            virtual ~KeyboardDeviceEm() {}

        private:
            InputSystemEm& inputSystemEm;
        };
    } // namespace input
} // namespace ouzel
