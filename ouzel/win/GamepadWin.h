// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <xinput.h>
#include "Gamepad.h"

namespace ouzel
{
    class InputWin;

    class GamepadWin: public Gamepad
    {
        friend InputWin;
    public:
        void update(const XINPUT_STATE& state);

        virtual int32_t getPlayerIndex() const override;

        virtual void setVibration(Motor motor, float speed) override;
        virtual float getVibration(Motor motor) override;
        
    protected:
        GamepadWin(int32_t playerIndex);

        void checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton);
        void checkButton(const XINPUT_STATE& state, WORD mask, GamepadButton button);

        XINPUT_STATE _state;
        XINPUT_VIBRATION _vibration;
        int32_t _playerIndex;
    };
}
