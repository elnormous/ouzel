// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <emscripten.h>
#include "ApplicationEm.h"
#include "core/Engine.h"
#include "utils/Utils.h"

static void loop(void)
{
    if (ouzel::sharedEngine->isActive())
    {
        static_cast<ouzel::ApplicationEm*>(ouzel::sharedApplication)->step();
    }
    else
    {
        emscripten_cancel_main_loop();
    }
}

namespace ouzel
{
    ApplicationEm::ApplicationEm(int aArgc, char* aArgv[]):
        Application(aArgc, aArgv)
    {
    }

    int ApplicationEm::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        sharedEngine->begin();

        emscripten_set_main_loop(loop, 0, 1);

        sharedEngine->end();

        return EXIT_SUCCESS;
    }

    void ApplicationEm::step()
    {
        executeAll();

        if (!sharedEngine->draw())
        {
            sharedEngine->exit();
        }
    }
}
