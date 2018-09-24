// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemEm;

        class GamepadEm final
        {
        public:
            GamepadEm(InputSystemEm& initInputSystemEm,
                      uint32_t initDeviceId,
                      long initIndex);

            inline uint32_t getDeviceId() const { return deviceId; }

            void update();

            long getIndex() const { return index; }

        private:
            void handleThumbAxisChange(double oldValue, double newValue,
                                       Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            InputSystemEm& inputSystemEm;
            uint32_t deviceId;
            long index = 0;
            double axis[64];
            double analogButton[64];
        };
    } // namespace input
} // namespace ouzel
