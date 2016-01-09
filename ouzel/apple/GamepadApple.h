// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Gamepad.h"
#include "Event.h"

#ifdef __OBJC__
@class GCController;
@class GCControllerDirectionPad;
@class GCControllerButtonInput;
#else
class GCController;
class GCControllerDirectionPad;
class GCControllerButtonInput;
#endif

namespace ouzel
{
    class InputApple;
    
    class GamepadApple: public Gamepad, public std::enable_shared_from_this<GamepadApple>
    {
        friend InputApple;
    public:
        virtual bool isAttached() const override;
        virtual void setAbsoluteDpadValues(bool absoluteDpadValues) override;
        virtual bool isAbsoluteDpadValues() const override;
        
        virtual int32_t getPlayerIndex() const override;
        virtual bool setPlayerIndex(int32_t playerIndex) override;
        
        GCController* getController() const { return _controller; }
        
    protected:
        GamepadApple(GCController* controller);
        
        void handleButtonValueChange(GamepadButton button, bool pressed, float value);
        
        GCController* _controller;
    };
}
