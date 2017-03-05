// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <queue>
#include <set>
#include <cstdint>
#include "utils/Noncopyable.h"
#include "events/Event.h"
#include "events/EventHandler.h"

namespace ouzel
{
    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();

        void dispatchEvents();

        void addEventHandler(const EventHandler* eventHandler);
        void removeEventHandler(const EventHandler* eventHandler);

        void postEvent(const Event& event);

    protected:
        EventDispatcher();

        std::vector<const EventHandler*> eventHandlers;
        std::set<const EventHandler*> eventHandlerAddSet;
        std::set<const EventHandler*> eventHandlerDeleteSet;

        std::mutex eventQueueMutex;
        std::queue<Event> eventQueue;
    };
}
