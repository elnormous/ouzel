// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "EventHandler.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    EventHandler::~EventHandler()
    {
        if (eventDispatcher) eventDispatcher->removeEventHandler(this);
    }

    void EventHandler::remove()
    {
        if (eventDispatcher)
        {
            eventDispatcher->removeEventHandler(this);
            eventDispatcher = nullptr;
        }
    }
}
