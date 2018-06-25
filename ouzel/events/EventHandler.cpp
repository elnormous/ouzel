// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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
