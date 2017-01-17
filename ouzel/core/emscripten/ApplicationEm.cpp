// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <emscripten.h>
#include "ApplicationEm.h"
#include "core/Engine.h"
#include "utils/Utils.h"

static void loop(void)
{
    if (!static_cast<ouzel::ApplicationEm*>(ouzel::sharedApplication)->step())
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

        emscripten_set_main_loop(loop, 0, 1);

        exit();

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->exitUpdateThread();
        }

        return EXIT_SUCCESS;
    }

    bool ApplicationEm::step()
    {
        executeAll();

        if (!sharedEngine->draw())
        {
            return false;
        }

        return active;
    }

    bool ApplicationEm::openURL(const std::string& url)
    {
        EM_ASM_ARGS({window.open(Pointer_stringify($0));}, url.c_str());

        return true;
    }
}
