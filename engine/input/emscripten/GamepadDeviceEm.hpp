// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEEM_HPP
#define OUZEL_INPUT_GAMEPADDEVICEEM_HPP

#include "input/GamepadDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm final: public GamepadDevice
        {
        public:
            GamepadDeviceEm(InputSystem& initInputSystem,
                            DeviceId initId,
                            long initIndex);

            void update();

            inline auto getIndex() const noexcept { return index; }

        private:
            void handleThumbAxisChange(double oldValue, double newValue,
                                       Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            long index = 0;
            double axis[64]{0.0};
            double analogButton[64]{0.0};
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEEM_HPP
