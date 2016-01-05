// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "ReferenceCounted.h"
#include "Event.h"

namespace ouzel
{
    class EventHandler: public ReferenceCounted
    {
    public:
        virtual bool handleEvent(const Event& event) = 0;
    };
}
