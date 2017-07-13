// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "Application.h"

namespace ouzel
{
    ouzel::Application* sharedApplication = nullptr;

    Application::Application():
        screenSaverEnabled(false),
        engine(new Engine())
    {
        sharedApplication = this;
    }

    Application::Application(int aArgc, char* aArgv[]):
        argc(aArgc), argv(aArgv),
        engine(new Engine())
    {
        sharedApplication = this;

        for (int i = 0; i < argc; ++i)
        {
            args.push_back(argv[i]);
        }
    }

    Application::Application(const std::vector<std::string>& aArgs):
        args(aArgs),
        engine(new Engine())
    {
        sharedApplication = this;
    }

    Application::~Application()
    {
        sharedApplication = nullptr;
    }

    bool Application::init()
    {
        engine->init();

        return true;
    }

    int Application::run()
    {
        return EXIT_SUCCESS;
    }

    void Application::exit()
    {
        active = false;

        if (engine)
        {
            engine.reset();
        }
    }

    bool Application::openURL(const std::string&)
    {
        return false;
    }

    void Application::setScreenSaverEnabled(bool newScreenSaverEnabled)
    {
        screenSaverEnabled = newScreenSaverEnabled;
    }
}
