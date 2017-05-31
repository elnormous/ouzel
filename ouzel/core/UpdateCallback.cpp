// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "UpdateCallback.h"
#include "Engine.h"

namespace ouzel
{
    UpdateCallback::~UpdateCallback()
    {
        if (engine) engine->unscheduleUpdate(this);
    }

    void UpdateCallback::remove()
    {
        if (engine)
        {
            engine->unscheduleUpdate(this);
            engine = nullptr;
        }
    }
}
