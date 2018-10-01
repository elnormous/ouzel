// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <Xinput.h>
#include "input/Gamepad.hpp"
#include "input/windows/GamepadDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceXI: public GamepadDeviceWin
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
