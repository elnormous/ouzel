// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <future>
#include <queue>
#include <set>
#include <vector>
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

        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);

        // dispatches the event immediately
        bool dispatchEvent(const Event& event);

        // posts the event for dispatching on the game thread
        std::future<bool> postEvent(const Event& event);

    protected:
        EventDispatcher();

    private:
        // dispatches all queued events on the game thread
        void dispatchEvents();

        std::vector<EventHandler*> eventHandlers;
        std::set<EventHandler*> eventHandlerAddSet;
        std::set<EventHandler*> eventHandlerDeleteSet;

        std::mutex eventQueueMutex;
        std::queue<std::pair<std::promise<bool>, Event>> eventQueue;
    };
}
