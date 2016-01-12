// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include <set>
#include "Types.h"
#include "Noncopyable.h"
#include "Event.h"
#include "EventHander.h"

namespace ouzel
{
    class EventHandler;
    
    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();
        
        void addEventHandler(EventHandlerPtr const& eventHandler);
        void removeEventHandler(EventHandlerPtr const& eventHandler);
        
        void dispatchKeyDownEvent(const KeyboardEvent& event, VoidPtr const& sender);
        void dispatchKeyUpEvent(const KeyboardEvent& event, VoidPtr const& sender);
        
        void dispatchMouseDownEvent(const MouseEvent& event, VoidPtr const& sender);
        void dispatchMouseUpEvent(const MouseEvent& event, VoidPtr const& sender);
        void dispatchMouseScrollEvent(const MouseEvent& event, VoidPtr const& sender);
        void dispatchMouseMoveEvent(const MouseEvent& event, VoidPtr const& sender);
        
        void dispatchTouchBeginEvent(const TouchEvent& event, VoidPtr const& sender);
        void dispatchTouchMoveEvent(const TouchEvent& event, VoidPtr const& sender);
        void dispatchTouchEndEvent(const TouchEvent& event, VoidPtr const& sender);
        void dispatchTouchCancelEvent(const TouchEvent& event, VoidPtr const& sender);
        
        void dispatchGamepadConnectEvent(const GamepadEvent& event, VoidPtr const& sender);
        void dispatchGamepadDisconnectEvent(const GamepadEvent& event, VoidPtr const& sender);
        void dispatchGamepadButtonChangeEvent(const GamepadEvent& event, VoidPtr const& sender);
        
        void dispatchWindowSizeChangeEvent(const WindowEvent& event, VoidPtr const& sender);
        void dispatchWindowTitleChangeEvent(const WindowEvent& event, VoidPtr const& sender);
        
    protected:
        EventDispatcher();
        
        void lock();
        void unlock();
        
        std::vector<EventHandlerPtr> _eventHandlers;
        std::set<EventHandlerPtr> _eventHandlerDeleteList;
        
        bool _locked = false;
    };
}
