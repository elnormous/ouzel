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
        sharedEngine->begin();

        while (sharedEngine->isActive())
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                sharedEngine->exit();
            }

            input->update();
        }

        sharedEngine->end();

        return EXIT_SUCCESS;
    }
}
