// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "EventHandler.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"

namespace ouzel
{
    EventHandler::~EventHandler()
    {
        sharedEngine->getEventDispatcher()->removeEventHandler(this);
    }
}
