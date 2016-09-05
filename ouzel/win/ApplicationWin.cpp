// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "ApplicationWin.h"
#include "InputWin.h"
#include "WindowWin.h"
#include "core/Engine.h"

namespace ouzel
{
    ApplicationWin::ApplicationWin(const std::vector<std::string>& pArgs):
        Application(pArgs)
    {
    }

    int ApplicationWin::run()
    {
        ouzelMain(args);

        if (!sharedEngine)
        {
            return 1;
        }

        std::shared_ptr<InputWin> input = std::static_pointer_cast<InputWin>(sharedEngine->getInput());
        std::shared_ptr<WindowWin> window = std::static_pointer_cast<WindowWin>(sharedEngine->getWindow());
        sharedEngine->begin();

        MSG msg;

        while (sharedEngine->isActive())
        {
            input->update();
            executeAll();

            if (!sharedEngine->draw())
            {
                sharedEngine->exit();
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
                        sharedEngine->exit();
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
                    sharedEngine->exit();
                    break;
                }
            }
        }

        sharedEngine->end();

        return 0;
    }
}
