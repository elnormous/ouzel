// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include <set>
#include <cstdint>
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
        
        void addEventHandler(const EventHandlerPtr& eventHandler);
        void removeEventHandler(const EventHandlerPtr& eventHandler);
        
        void dispatchKeyDownEvent(const KeyboardEvent& event, const VoidPtr& sender);
        void dispatchKeyRepeatEvent(const KeyboardEvent& event, const VoidPtr& sender);
        void dispatchKeyUpEvent(const KeyboardEvent& event, const VoidPtr& sender);
        
        void dispatchMouseDownEvent(const MouseEvent& event, const VoidPtr& sender);
        void dispatchMouseUpEvent(const MouseEvent& event, const VoidPtr& sender);
        void dispatchMouseScrollEvent(const MouseEvent& event, const VoidPtr& sender);
        void dispatchMouseMoveEvent(const MouseEvent& event, const VoidPtr& sender);
        
        void dispatchTouchBeginEvent(const TouchEvent& event, const VoidPtr& sender);
        void dispatchTouchMoveEvent(const TouchEvent& event, const VoidPtr& sender);
        void dispatchTouchEndEvent(const TouchEvent& event, const VoidPtr& sender);
        void dispatchTouchCancelEvent(const TouchEvent& event, const VoidPtr& sender);
        
        void dispatchGamepadConnectEvent(const GamepadEvent& event, const VoidPtr& sender);
        void dispatchGamepadDisconnectEvent(const GamepadEvent& event, const VoidPtr& sender);
        void dispatchGamepadButtonChangeEvent(const GamepadEvent& event, const VoidPtr& sender);
        
        void dispatchWindowSizeChangeEvent(const WindowEvent& event, const VoidPtr& sender);
        void dispatchWindowTitleChangeEvent(const WindowEvent& event, const VoidPtr& sender);
        
    protected:
        EventDispatcher();
        
        void lock();
        void unlock();
        
        std::vector<EventHandlerPtr> _eventHandlers;
        std::set<EventHandlerPtr> _eventHandlerDeleteList;
        
        int32_t _locked = 0;
    };
}
