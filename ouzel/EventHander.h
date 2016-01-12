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
        
        std::function<bool(const KeyboardEvent&, VoidPtr const&)> keyDownHandler;
        std::function<bool(const KeyboardEvent&, VoidPtr const&)> keyUpHandler;
        
        std::function<bool(const MouseEvent&, VoidPtr const&)> mouseDownHandler;
        std::function<bool(const MouseEvent&, VoidPtr const&)> mouseUpHandler;
        std::function<bool(const MouseEvent&, VoidPtr const&)> mouseScrollHandler;
        std::function<bool(const MouseEvent&, VoidPtr const&)> mouseMoveHandler;
        
        std::function<bool(const TouchEvent&, VoidPtr const&)> touchBeginHandler;
        std::function<bool(const TouchEvent&, VoidPtr const&)> touchMoveHandler;
        std::function<bool(const TouchEvent&, VoidPtr const&)> touchEndHandler;
        std::function<bool(const TouchEvent&, VoidPtr const&)> touchCancelHandler;
        
        std::function<bool(const GamepadEvent&, VoidPtr const&)> gamepadConnectHandler;
        std::function<bool(const GamepadEvent&, VoidPtr const&)> gamepadDisconnectHandler;
        std::function<bool(const GamepadEvent&, VoidPtr const&)> gamepadButtonChangeHandler;
        
        std::function<bool(const WindowEvent&, VoidPtr const&)> windowSizeChangeHandler;
        std::function<bool(const WindowEvent&, VoidPtr const&)> windowTitleChangeHandler;
        
    protected:
        int32_t _priority;
    };
}
