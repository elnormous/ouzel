// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Event.h"

namespace ouzel
{
    class EventHandler
    {
    public:
        virtual bool handleEvent(const Event& event) = 0;
        virtual void update(float delta) = 0;
    };
}
