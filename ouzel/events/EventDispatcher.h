// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "events/Event.h"
#include "events/EventHander.h"

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

        void dispatchEvent(const Event& event);

    protected:
        EventDispatcher();

        std::list<EventHandlerPtr> eventHandlers;
        std::list<EventHandlerPtr>::iterator eventHandlerIterator;
        std::queue<Event> eventQueue;
        std::mutex mutex;
    };
}
