// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <xinput.h>
#include "Input.h"

namespace ouzel
{
    class Engine;
    class GamepadWin;
    
    class InputWin: public Input
    {
        friend Engine;
    public:
        
        virtual ~InputWin();

        virtual void update() override;
        
    protected:
        InputWin();

        std::shared_ptr<GamepadWin> _gamepads[XUSER_MAX_COUNT];
    };
}
