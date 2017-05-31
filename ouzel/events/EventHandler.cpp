// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "EventHandler.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"

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
