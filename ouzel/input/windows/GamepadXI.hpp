// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManagerWin;

        class GamepadXI: public Gamepad
        {
            friend InputManagerWin;
        public:
            void update();

            virtual int32_t getPlayerIndex() const override;

            virtual void setVibration(Motor motor, float speed) override;
            virtual float getVibration(Motor motor) override;

        protected:
            explicit GamepadXI(DWORD aPlayerIndex);

            void checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton);
            void checkButton(const XINPUT_STATE& newState, WORD mask, GamepadButton button);

            XINPUT_STATE state;
            XINPUT_VIBRATION vibration;
            DWORD playerIndex;
        };
    } // namespace input
} // namespace ouzel
