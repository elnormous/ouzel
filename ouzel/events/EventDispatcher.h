// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include <set>
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

        std::vector<EventHandlerPtr> eventHandlers;
        std::queue<Event> eventQueue;
        std::mutex mutex;
    };
}
