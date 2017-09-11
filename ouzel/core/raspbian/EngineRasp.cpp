// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <pthread.h>
#include "EngineRasp.hpp"
#include "input/raspbian/InputRasp.hpp"
#include "utils/Utils.hpp"

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

            inputRasp->update();
        }

        exit();

        return EXIT_SUCCESS;
    }

    void EngineRasp::executeOnMainThread(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    bool EngineRasp::setCurrentThreadName(const std::string& name)
    {
        return pthread_setname_np(pthread_self(), name.c_str()) == 0;
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
