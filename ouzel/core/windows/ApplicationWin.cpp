// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include <cstdlib>
#include "ApplicationWin.h"
#include "input/windows/InputWin.h"
#include "WindowWin.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    ApplicationWin::ApplicationWin(const std::vector<std::string>& pArgs):
        Application(pArgs)
    {
    }

	ApplicationWin::~ApplicationWin()
	{
		//CoUninitialize();
	}

    int ApplicationWin::run()
    {
		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr))
		{
			Log(Log::Level::ERR) << "Failed to initialize COM";
			return EXIT_FAILURE;
		}

        ouzelMain(args);

        if (!sharedEngine)
        {
            return EXIT_FAILURE;
        }

        std::shared_ptr<input::InputWin> input = std::static_pointer_cast<input::InputWin>(sharedEngine->getInput());
        std::shared_ptr<WindowWin> window = std::static_pointer_cast<WindowWin>(sharedEngine->getWindow());
        sharedEngine->begin();

        MSG msg;

        while (sharedEngine->isActive())
        {
            executeAll();

            if (!sharedEngine->draw())
            {
                sharedEngine->exit();
            }

            std::set<HACCEL> accelerators = window->getAccelerators();

            while (sharedEngine->isActive())
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

            input->update();
        }

        sharedEngine->end();

        return EXIT_SUCCESS;
    }
}
