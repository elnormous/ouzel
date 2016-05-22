// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#include <xinput.h>
#include "Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class InputWin;

        class GamepadWin : public Gamepad
        {
            friend InputWin;
        public:
            void update(const XINPUT_STATE& newState);

            virtual int32_t getPlayerIndex() const override;

            virtual void setVibration(Motor motor, float speed) override;
            virtual float getVibration(Motor motor) override;

        protected:
            GamepadWin(int32_t pPlayerIndex);

            void checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton);
            void checkButton(const XINPUT_STATE& newState, WORD mask, GamepadButton button);

            XINPUT_STATE state;
            XINPUT_VIBRATION vibration;
            int32_t playerIndex;
        };
    } // namespace input
} // namespace ouzel
