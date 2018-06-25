// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <queue>
#include <set>
#include <cstdint>
#include "events/Event.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    class EventDispatcher final
    {
        friend Engine;
    public:
        ~EventDispatcher();

        EventDispatcher(const EventDispatcher&) = delete;
        EventDispatcher& operator=(const EventDispatcher&) = delete;

        EventDispatcher(EventDispatcher&&) = delete;
        EventDispatcher& operator=(EventDispatcher&&) = delete;

        void dispatchEvents();

        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);

        void postEvent(const Event& event, bool dispatchImmediately = false);

    protected:
        EventDispatcher();

    private:
        void dispatchEvent(const Event& event);

        std::vector<EventHandler*> eventHandlers;
        std::set<EventHandler*> eventHandlerAddSet;
        std::set<EventHandler*> eventHandlerDeleteSet;

        Mutex eventQueueMutex;
        std::queue<Event> eventQueue;
    };
}
