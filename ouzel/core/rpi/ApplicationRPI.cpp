// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationRPI.h"
#include "core/Engine.h"
#include "input/rpi/InputRPI.h"
#include "utils/Utils.h"

namespace ouzel
{
    ApplicationRPI::ApplicationRPI(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    int ApplicationRPI::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return 1;
        }

        std::shared_ptr<input::InputRPI> input = std::static_pointer_cast<input::InputRPI>(sharedEngine->getInput());
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

        return 0;
    }
}
