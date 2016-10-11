// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <queue>
#include <set>
#include <cstdint>
#include "utils/Types.h"
#include "utils/Noncopyable.h"
#include "events/Event.h"
#include "events/EventHandler.h"

namespace ouzel
{
    class EventHandler;

    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();

        void dispatchEvents();

        void addEventHandler(const EventHandler& eventHandler);
        void removeEventHandler(const EventHandler& eventHandler);

        void postEvent(const Event& event);

    protected:
        EventDispatcher();

        std::vector<const EventHandler*> eventHandlers;
        std::set<const EventHandler*> eventHandlerAddSet;

        std::mutex eventQueueMutex;
        std::queue<Event> eventQueue;
    };
}
