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
#include "events/EventHandler.h"

namespace ouzel
{
    class EventHandler;

    class EventDispatcher: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~EventDispatcher();

        void update();

        void addEventHandler(const EventHandler& eventHandler);
        void removeEventHandler(const EventHandler& eventHandler);

        void dispatchEvent(const Event& event);

    protected:
        EventDispatcher();

        std::list<const EventHandler*> eventHandlers;
        std::list<const EventHandler*>::iterator eventHandlerIterator;
        bool eventHandlerDeleted = false;
        std::queue<Event> eventQueue;
        std::mutex mutex;
    };
}
