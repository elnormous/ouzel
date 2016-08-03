// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include "Application.h"

namespace ouzel
{
    ouzel::Application* sharedApplication = nullptr;

    Application::Application(int pArgc, char* pArgv[])
    {
        sharedApplication = this;

        argc = pArgc;
        argv = pArgv;

        args.clear();

        for (int32_t i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    Application::Application(const std::vector<std::string>& pArgs):
        args(pArgs)
    {
        sharedApplication = this;
    }

    Application::~Application()
    {
    }

    bool Application::run()
    {
        return true;
    }
}
