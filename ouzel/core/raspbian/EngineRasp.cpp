// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <pthread.h>
#include "EngineRasp.hpp"
#include "input/raspbian/InputManagerRasp.hpp"
#include "thread/Lock.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    EngineRasp::EngineRasp(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);
    }

    void EngineRasp::run()
    {
        init();
        start();

        input::InputManagerRasp* inputRasp = static_cast<input::InputManagerRasp*>(inputManager.get());

        while (active)
        {
            executeAll();

            inputRasp->update();
        }

        exit();
    }

    void EngineRasp::executeOnMainThread(const std::function<void(void)>& func)
    {
        Lock lock(executeMutex);

        executeQueue.push(func);
    }

    void EngineRasp::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                Lock lock(executeMutex);

                if (executeQueue.empty())
                    break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func) func();
        }
    }
}
