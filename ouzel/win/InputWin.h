// Copyright (C) 2016 Elviss Strazdins
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
        
        virtual void setCursorVisible(bool visible) override;
        virtual bool isCursorVisible() const override;

    protected:
        InputWin();

        std::shared_ptr<GamepadWin> _gamepads[XUSER_MAX_COUNT];

        bool _cursorVisible = true;
    };
}
