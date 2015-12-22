// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

namespace ouzel
{
    enum class EventType
    {
        EVENT_TYPE_KEY_DOWN,
        EVENT_TYPE_KEY_UP,
        EVENT_TYPE_MOUSE_DOWN,
        EVENT_TYPE_MOUSE_UP,
        EVENT_TYPE_MOUSE_SCROLL,
        EVENT_TYPE_MOUSE_MOVE,
        EVENT_TYPE_MOUSE_DRAG
    };
    
    struct Event
    {
        EventType type;
        
        uint32_t key;
        uint32_t mouseButton;
        Vector2 mousePosition;
        Vector2 mouseScroll;
        
        bool shiftDown;
        bool controlDown;
        bool commandDown;
        bool functionDown;
    };
    
    class EventHandler
    {
    public:
        virtual bool handleEvent(const Event& event) = 0;
        virtual void update(float delta) = 0;
    };
}
