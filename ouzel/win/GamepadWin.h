// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <xinput.h>
#include "Gamepad.h"
#include "Event.h"

namespace ouzel
{
    class InputWin;

    class GamepadWin: public Gamepad, public std::enable_shared_from_this<GamepadWin>
    {
        friend InputWin;
    public:
        void update(XINPUT_STATE const& state);

        virtual int32_t getPlayerIndex() const override;

    protected:
        GamepadWin(int32_t playerIndex);

        void checkButton(XINPUT_STATE const& state, WORD mask, GamepadButton button);
        void handleButtonValueChange(GamepadButton button, bool pressed, float value);

        XINPUT_STATE _state;
        int32_t _playerIndex;
    };
}
