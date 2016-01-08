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
        
        std::function<void(const KeyboardEvent&, std::shared_ptr<void> const&)> keyDownHandler;
        std::function<void(const KeyboardEvent&, std::shared_ptr<void> const&)> keyUpHandler;
        
        std::function<void(const MouseEvent&, std::shared_ptr<void> const&)> mouseDownHandler;
        std::function<void(const MouseEvent&, std::shared_ptr<void> const&)> mouseUpHandler;
        std::function<void(const MouseEvent&, std::shared_ptr<void> const&)> mouseScrollHandler;
        std::function<void(const MouseEvent&, std::shared_ptr<void> const&)> mouseMoveHandler;
        std::function<void(const MouseEvent&, std::shared_ptr<void> const&)> mouseDragHandler;
        
        std::function<void(const TouchEvent&, std::shared_ptr<void> const&)> touchBeginHandler;
        std::function<void(const TouchEvent&, std::shared_ptr<void> const&)> touchMoveHandler;
        std::function<void(const TouchEvent&, std::shared_ptr<void> const&)> touchEndHandler;
        std::function<void(const TouchEvent&, std::shared_ptr<void> const&)> touchCancelHandler;
        
        std::function<void(const GamepadEvent&, std::shared_ptr<void> const&)> gamepadConnectHandler;
        std::function<void(const GamepadEvent&, std::shared_ptr<void> const&)> gamepadDisconnectHandler;
        std::function<void(const GamepadEvent&, std::shared_ptr<void> const&)> gamepadButtonChangeHandler;
        
        std::function<void(const ScreenSizeEvent&, std::shared_ptr<void> const&)> screenSizeHandler;
        
    protected:
        int32_t _priority;
    };
}
