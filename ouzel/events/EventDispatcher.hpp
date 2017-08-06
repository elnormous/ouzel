// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <mutex>
#include <queue>
#include <set>
#include <cstdint>
#include "utils/Noncopyable.hpp"
#include "events/Event.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();

        void dispatchEvents();

        void addEventHandler(EventHandler* eventHandler);
        void removeEventHandler(EventHandler* eventHandler);

        void postEvent(const Event& event);

    protected:
        EventDispatcher();

        std::vector<EventHandler*> eventHandlers;
        std::set<EventHandler*> eventHandlerAddSet;
        std::set<EventHandler*> eventHandlerDeleteSet;

        std::mutex eventQueueMutex;
        std::queue<Event> eventQueue;
    };
}
