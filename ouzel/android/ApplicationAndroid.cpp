// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationAndroid.h"
#include "core/Engine.h"

namespace ouzel
{
    ApplicationAndroid::ApplicationAndroid()
    {
    }
    
    int ApplicationAndroid::run()
    {
        ouzelMain(ouzel::sharedApplication->getArgs());

        if (!sharedEngine)
        {
            return 1;
        }

        sharedEngine->begin();

        return 0;
    }
}