// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "input/Gamepad.hpp"
#include "input/windows/GamepadWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadXI: public GamepadWin
        {
        public:
            GamepadXI(InputSystemWin& initInputSystemWin,
                      uint32_t initDeviceId,
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
