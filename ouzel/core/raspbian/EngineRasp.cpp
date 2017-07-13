// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "EngineRasp.h"
#include "input/raspbian/InputRasp.h"
#include "utils/Utils.h"

namespace ouzel
{
    EngineRasp::EngineRasp(int aArgc, char* aArgv[])
    {
        for (int i = 0; i < aArgc; ++i)
        {
            args.push_back(aArgv[i]);
        }
    }

    int EngineRasp::run()
    {
        if (!init())
        {
            return EXIT_FAILURE;
        }

        start();

        input::InputRasp* inputRasp = static_cast<input::InputRasp*>(input.get());

        while (active)
        {
            executeAll();

            if (!active ||
                !renderer->process())
            {
                break;
            }

            inputRasp->update();
        }

        exit();

        return EXIT_SUCCESS;
    }

    void EngineRasp::execute(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    void EngineRasp::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                std::lock_guard<std::mutex> lock(executeMutex);

                if (executeQueue.empty())
                {
                    break;
                }

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }
}
