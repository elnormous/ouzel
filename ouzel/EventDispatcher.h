// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include <set>
#include <mutex>
#include <queue>
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

        void update();

        void addEventHandler(const EventHandlerPtr& eventHandler);
        void removeEventHandler(const EventHandlerPtr& eventHandler);

        void dispatchEvent(const EventPtr& event, const VoidPtr& sender);

    protected:
        EventDispatcher();

        void dispatchKeyboardEvent(const KeyboardEventPtr& event, const VoidPtr& sender);
        void dispatchMouseEvent(const MouseEventPtr& event, const VoidPtr& sender);
        void dispatchTouchEvent(const TouchEventPtr& event, const VoidPtr& sender);
        void dispatchGamepadEvent(const GamepadEventPtr& event, const VoidPtr& sender);
        void dispatchWindowEvent(const WindowEventPtr& event, const VoidPtr& sender);
        void dispatchSystemEvent(const SystemEventPtr& event, const VoidPtr& sender);
        void dispatchUIEvent(const UIEventPtr& event, const VoidPtr& sender);

        void lock();
        void unlock();

        std::vector<EventHandlerPtr> _eventHandlers;
        std::set<EventHandlerPtr> _eventHandlerAddList;
        std::set<EventHandlerPtr> _eventHandlerRemoveList;

        std::queue<std::pair<EventPtr, VoidPtr>> _eventQueue;
        std::mutex _mutex;
        int32_t _locked = 0;
    };
}
