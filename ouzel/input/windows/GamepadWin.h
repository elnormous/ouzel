// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>
#include "input/Gamepad.h"

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
            GamepadWin(IDirectInputDevice8* aDevice, DWORD aPlayerIndex);

            void checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton);
            void checkButton(const XINPUT_STATE& newState, WORD mask, GamepadButton button);

            IDirectInputDevice8* device = nullptr;
            XINPUT_STATE state;
            XINPUT_VIBRATION vibration;
            DWORD playerIndex;
        };
    } // namespace input
} // namespace ouzel
