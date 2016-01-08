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
        virtual ~EventDispatcher();
        
        void addEventHandler(std::shared_ptr<EventHandler> const& eventHandler);
        void removeEventHandler(std::shared_ptr<EventHandler> const& eventHandler);
        
        void dispatchKeyDownEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender);
        void dispatchKeyUpEvent(const KeyboardEvent& event, std::shared_ptr<void> const& sender);
        
        void dispatchMouseDownEvent(const MouseEvent& event, std::shared_ptr<void> const& sender);
        void dispatchMouseUpEvent(const MouseEvent& event, std::shared_ptr<void> const& sender);
        void dispatchMouseScrollEvent(const MouseEvent& event, std::shared_ptr<void> const& sender);
        void dispatchMouseMoveEvent(const MouseEvent& event, std::shared_ptr<void> const& sender);
        void dispatchMouseDragEvent(const MouseEvent& event, std::shared_ptr<void> const& sender);
        
        void dispatchTouchBeginEvent(const TouchEvent& event, std::shared_ptr<void> const& sender);
        void dispatchTouchMoveEvent(const TouchEvent& event, std::shared_ptr<void> const& sender);
        void dispatchTouchEndEvent(const TouchEvent& event, std::shared_ptr<void> const& sender);
        void dispatchTouchCancelEvent(const TouchEvent& event, std::shared_ptr<void> const& sender);
        
        void dispatchGamepadConnectEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender);
        void dispatchGamepadDisconnectEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender);
        void dispatchGamepadButtonChangeEvent(const GamepadEvent& event, std::shared_ptr<void> const& sender);
        
        void dispatchScreenSizeEvent(const ScreenSizeEvent& event, std::shared_ptr<void> const& sender);
        
    protected:
        EventDispatcher();
        
        std::vector<std::shared_ptr<EventHandler>> _eventHandlers;
    };
}