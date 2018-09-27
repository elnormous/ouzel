// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemEm;

        class KeyboardEm: public InputDevice
        {
        public:
            KeyboardEm(InputSystemEm& initInputSystemEm,
                       uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemEm(initInputSystemEm)
            {
            }

            virtual ~KeyboardEm() {}

        private:
            InputSystemEm& inputSystemEm;
        };
    } // namespace input
} // namespace ouzel
