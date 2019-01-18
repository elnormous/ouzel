// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEXI_HPP
#define OUZEL_INPUT_GAMEPADDEVICEXI_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#include "input/Gamepad.hpp"
#include "input/windows/GamepadDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceXI final: public GamepadDeviceWin
        {
        public:
            GamepadDeviceXI(InputSystem& initInputSystem,
                            uint32_t initId,
                            DWORD aPlayerIndex);

            void update();

            int32_t getPlayerIndex() const;

            void setVibration(Gamepad::Motor motor, float speed);
            float getVibration(Gamepad::Motor motor);

        protected:

            void checkThumbAxis(SHORT oldValue, SHORT newValue, Gamepad::Button negativeButton, Gamepad::Button positiveButton);
            void checkButton(const XINPUT_STATE& newState, WORD mask, Gamepad::Button button);

            XINPUT_STATE state;
            XINPUT_VIBRATION vibration;
            DWORD playerIndex;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICEXI_HPP
