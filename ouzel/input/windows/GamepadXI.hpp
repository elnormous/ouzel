// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <xinput.h>
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class InputWin;

        class GamepadXI: public Gamepad
        {
            friend InputWin;
        public:
            void update(const XINPUT_STATE& newState);

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
