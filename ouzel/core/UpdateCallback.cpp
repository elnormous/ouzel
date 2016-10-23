// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "UpdateCallback.h"
#include "Engine.h"

namespace ouzel
{
    UpdateCallback::~UpdateCallback()
    {
        sharedEngine->unscheduleUpdate(this);
    }
}
