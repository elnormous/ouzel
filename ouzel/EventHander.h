// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "Event.h"

namespace ouzel
{
    class EventDispatcher;
    
    class EventHandler
    {
        friend EventDispatcher;
        
    public:
        static const int32_t PRIORITY_MAX = 0x1000;
        
        EventHandler(int32_t priority = 0): _priority(priority) { }
        
        std::function<bool(const KeyboardEvent&, const VoidPtr&)> keyDownHandler;
        std::function<bool(const KeyboardEvent&, const VoidPtr&)> keyUpHandler;
        
        std::function<bool(const MouseEvent&, const VoidPtr&)> mouseDownHandler;
        std::function<bool(const MouseEvent&, const VoidPtr&)> mouseUpHandler;
        std::function<bool(const MouseEvent&, const VoidPtr&)> mouseScrollHandler;
        std::function<bool(const MouseEvent&, const VoidPtr&)> mouseMoveHandler;
        
        std::function<bool(const TouchEvent&, const VoidPtr&)> touchBeginHandler;
        std::function<bool(const TouchEvent&, const VoidPtr&)> touchMoveHandler;
        std::function<bool(const TouchEvent&, const VoidPtr&)> touchEndHandler;
        std::function<bool(const TouchEvent&, const VoidPtr&)> touchCancelHandler;
        
        std::function<bool(const GamepadEvent&, const VoidPtr&)> gamepadConnectHandler;
        std::function<bool(const GamepadEvent&, const VoidPtr&)> gamepadDisconnectHandler;
        std::function<bool(const GamepadEvent&, const VoidPtr&)> gamepadButtonChangeHandler;
        
        std::function<bool(const WindowEvent&, const VoidPtr&)> windowSizeChangeHandler;
        std::function<bool(const WindowEvent&, const VoidPtr&)> windowTitleChangeHandler;
        
    protected:
        int32_t _priority;
    };
}
