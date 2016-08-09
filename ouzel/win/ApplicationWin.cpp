// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "ApplicationWin.h"
#include "WindowWin.h"
#include "core/Engine.h"

namespace ouzel
{
    ApplicationWin::ApplicationWin(const std::vector<std::string>& pArgs):
        Application(pArgs)
    {
    }

    bool ApplicationWin::run()
    {
        ouzelMain(args);

        if (!ouzel::sharedEngine)
        {
            return false;
        }

        std::shared_ptr<ouzel::WindowWin> window = std::static_pointer_cast<ouzel::WindowWin>(ouzel::sharedEngine->getWindow());
        sharedEngine->begin();

        MSG msg;

        bool running = true;

        while (running)
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                running = false;
            }

            std::set<HACCEL> accelerators = window->getAccelerators();

            for (;;)
            {
                if (sharedEngine->isRunning())
                {
                    if (!PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                    {
                        break;
                    }
                }
                else
                {
                    if (GetMessage(&msg, nullptr, 0, 0) <= 0)
                    {
                        running = false;
                        break;
                    }
                }

                bool translate = true;

                for (HACCEL accelerator : accelerators)
                {
                    if (TranslateAccelerator(window->getNativeWindow(), accelerator, &msg))
                    {
                        translate = false;
                    }
                }

                if (translate)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (msg.message == WM_QUIT)
                {
                    running = false;
                    break;
                }
            }
        }

        sharedEngine->end();

        return true;
    }
}
