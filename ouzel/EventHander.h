// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <functional>
#include "ReferenceCounted.h"
#include "Event.h"

namespace ouzel
{
    class EventHandler: public ReferenceCounted
    {
    public:
        std::function<void(const KeyboardEvent&, ReferenceCounted*)> keyDownHandler;
        std::function<void(const KeyboardEvent&, ReferenceCounted*)> keyUpHandler;
        
        std::function<void(const MouseEvent&, ReferenceCounted*)> mouseDownHandler;
        std::function<void(const MouseEvent&, ReferenceCounted*)> mouseUpHandler;
        std::function<void(const MouseEvent&, ReferenceCounted*)> mouseScrollHandler;
        std::function<void(const MouseEvent&, ReferenceCounted*)> mouseMoveHandler;
        std::function<void(const MouseEvent&, ReferenceCounted*)> mouseDragHandler;
        
        std::function<void(const TouchEvent&, ReferenceCounted*)> touchBeginHandler;
        std::function<void(const TouchEvent&, ReferenceCounted*)> touchMoveHandler;
        std::function<void(const TouchEvent&, ReferenceCounted*)> touchEndHandler;
        std::function<void(const TouchEvent&, ReferenceCounted*)> touchCancelHandler;
        
        std::function<void(const ScreenSizeEvent&, ReferenceCounted*)> screenSizeHandler;
    };
}
