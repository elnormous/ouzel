// Copyright (C) 2017 Elviss Strazdins
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

        if (sharedEngine)
        {
            sharedEngine->start();
        }
        else
        {
            return EXIT_FAILURE;
        }

        input::InputRasp* inputRasp = static_cast<input::InputRasp*>(sharedEngine->getInput());

        while (active)
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                break;
            }

            inputRasp->update();
        }

        if (sharedEngine)
        {
            sharedEngine->stop();
        }

        return EXIT_SUCCESS;
    }
}
