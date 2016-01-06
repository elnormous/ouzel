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
        
        std::function<void(const KeyboardEvent&, void*)> keyDownHandler;
        std::function<void(const KeyboardEvent&, void*)> keyUpHandler;
        
        std::function<void(const MouseEvent&, void*)> mouseDownHandler;
        std::function<void(const MouseEvent&, void*)> mouseUpHandler;
        std::function<void(const MouseEvent&, void*)> mouseScrollHandler;
        std::function<void(const MouseEvent&, void*)> mouseMoveHandler;
        std::function<void(const MouseEvent&, void*)> mouseDragHandler;
        
        std::function<void(const TouchEvent&, void*)> touchBeginHandler;
        std::function<void(const TouchEvent&, void*)> touchMoveHandler;
        std::function<void(const TouchEvent&, void*)> touchEndHandler;
        std::function<void(const TouchEvent&, void*)> touchCancelHandler;
        
        std::function<void(const ScreenSizeEvent&, void*)> screenSizeHandler;
        
    protected:
        int32_t _priority;
    };
}
