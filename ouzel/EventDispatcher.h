// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include "Noncopyable.h"
#include "Event.h"
#include "EventHander.h"

namespace ouzel
{
    class Engine;
    class EventHandler;
    
    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        static EventDispatcher* getInstance();
        
        virtual ~EventDispatcher();
        
        void addEventHandler(std::shared_ptr<EventHandler> const& eventHandler);
        void removeEventHandler(std::shared_ptr<EventHandler> const& eventHandler);
        
        void dispatchKeyDownEvent(const KeyboardEvent& event, void* sender);
        void dispatchKeyUpEvent(const KeyboardEvent& event, void* sender);
        
        void dispatchMouseDownEvent(const MouseEvent& event, void* sender);
        void dispatchMouseUpEvent(const MouseEvent& event, void* sender);
        void dispatchMouseScrollEvent(const MouseEvent& event, void* sender);
        void dispatchMouseMoveEvent(const MouseEvent& event, void* sender);
        void dispatchMouseDragEvent(const MouseEvent& event, void* sender);
        
        void dispatchTouchBeginEvent(const TouchEvent& event, void* sender);
        void dispatchTouchMoveEvent(const TouchEvent& event, void* sender);
        void dispatchTouchEndEvent(const TouchEvent& event, void* sender);
        void dispatchTouchCancelEvent(const TouchEvent& event, void* sender);
        
        void dispatchScreenSizeEvent(const ScreenSizeEvent& event, void* sender);
        
    protected:
        EventDispatcher();
        
        std::vector<std::shared_ptr<EventHandler>> _eventHandlers;
    };
}