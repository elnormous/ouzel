// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationEm.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    ApplicationEm::ApplicationEm(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    int ApplicationEm::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return 1;
        }

        sharedEngine->begin();

        while (sharedEngine->isActive())
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                sharedEngine->exit();
            }
        }

        sharedEngine->end();

        return 0;
    }
}
