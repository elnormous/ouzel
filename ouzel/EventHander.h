//
//  EventHander.h
//  Ouzel
//
//  Created by Elviss Strazdins on 21/03/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

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
        virtual void update() = 0;
    };
}
