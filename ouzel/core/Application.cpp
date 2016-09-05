// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include "Application.h"
#include "files/FileSystem.h"

namespace ouzel
{
    ouzel::Application* sharedApplication = nullptr;

    Application::Application()
    {
        sharedApplication = this;

        fileSystem.reset(new FileSystem());
    }

    Application::Application(int pArgc, char* pArgv[]):
        Application()
    {
        argc = pArgc;
        argv = pArgv;

        args.clear();

        for (int32_t i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    Application::Application(const std::vector<std::string>& pArgs):
        Application()
    {
        args = pArgs;
    }

    Application::~Application()
    {
    }

    int Application::run()
    {
        return 0;
    }

    void Application::execute(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    void Application::executeAll()
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

                func = executeQueue.front();
                executeQueue.pop();
            }

            if (func)
            {
                func();
            }
        }
    }
}
