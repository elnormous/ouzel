// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "ApplicationAndroid.h"
#include "core/Engine.h"

namespace ouzel
{
    ApplicationAndroid::ApplicationAndroid(jobject aMainActivity, AAssetManager* aAssetManager):
        mainActivity(aMainActivity), assetManager(aAssetManager)
    {
    }

    int ApplicationAndroid::run()
    {
        ouzelMain(ouzel::sharedApplication->getArgs());

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        sharedEngine->begin();

        return EXIT_SUCCESS;
    }
}