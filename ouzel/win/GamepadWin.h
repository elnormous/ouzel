// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Gamepad.h"

namespace ouzel
{
    class GamepadWin: public Gamepad
    {
    public:
        GamepadWin();
        
        virtual int32_t getPlayerIndex() const override;
        virtual void setPlayerIndex(int32_t playerIndex) override;
    };
}
