// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "ApplicationRasp.h"
#include "core/Engine.h"
#include "input/raspbian/InputRasp.h"
#include "utils/Utils.h"

namespace ouzel
{
    ApplicationRasp::ApplicationRasp(int aArgc, char* aArgv[]):
        Application(aArgc, aArgv)
    {
    }

    int ApplicationRasp::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        input::InputRasp* input = static_cast<input::InputRasp*>(sharedEngine->getInput());

        while (active)
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                break;
            }

            input->update();
        }

        if (ouzel::sharedEngine)
        {
            ouzel::sharedEngine->exitUpdateThread();
        }

        return EXIT_SUCCESS;
    }
}
