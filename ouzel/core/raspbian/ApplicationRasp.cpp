// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationRasp.h"
#include "core/Engine.h"
#include "input/raspbian/InputRasp.h"
#include "utils/Utils.h"

namespace ouzel
{
    ApplicationRasp::ApplicationRasp(int pArgc, char* pArgv[]):
        Application(pArgc, pArgv)
    {
    }

    int ApplicationRasp::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return 1;
        }

        std::shared_ptr<input::InputRasp> input = std::static_pointer_cast<input::InputRasp>(sharedEngine->getInput());
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
