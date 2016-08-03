// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationRPI.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    ApplicationRPI::ApplicationRPI(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    bool ApplicationRPI::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return false;
        }

        sharedEngine->begin();

        for (;;)
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                break;
            }
        }

        sharedEngine->end();

        return true;
    }
}
