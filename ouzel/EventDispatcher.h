// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "SharedPtr.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"
#include "Event.h"
#include "EventHander.h"

namespace ouzel
{
    class Engine;
    class EventHandler;
    
    class EventDispatcher: public Noncopyable, public ReferenceCounted
    {
        friend Engine;
    public:
        static EventDispatcher* getInstance();
        
        virtual ~EventDispatcher();
        
        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);
        
        void dispatchKeyDownEvent(const KeyboardEvent& event, ReferenceCounted* sender);
        void dispatchKeyUpEvent(const KeyboardEvent& event, ReferenceCounted* sender);
        
        void dispatchMouseDownEvent(const MouseEvent& event, ReferenceCounted* sender);
        void dispatchMouseUpEvent(const MouseEvent& event, ReferenceCounted* sender);
        void dispatchMouseScrollEvent(const MouseEvent& event, ReferenceCounted* sender);
        void dispatchMouseMoveEvent(const MouseEvent& event, ReferenceCounted* sender);
        void dispatchMouseDragEvent(const MouseEvent& event, ReferenceCounted* sender);
        
        void dispatchTouchBeginEvent(const TouchEvent& event, ReferenceCounted* sender);
        void dispatchTouchMoveEvent(const TouchEvent& event, ReferenceCounted* sender);
        void dispatchTouchEndEvent(const TouchEvent& event, ReferenceCounted* sender);
        void dispatchTouchCancelEvent(const TouchEvent& event, ReferenceCounted* sender);
        
        void dispatchScreenSizeEvent(const ScreenSizeEvent& event, ReferenceCounted* sender);
        
    protected:
        EventDispatcher();
        
        std::vector<SharedPtr<EventHandler>> _eventHandlers;
    };
}