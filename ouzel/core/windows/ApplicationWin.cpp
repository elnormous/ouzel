// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include <cstdlib>
#include "ApplicationWin.h"
#include "WindowWin.h"
#include "input/windows/InputWin.h"
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
            Log(Log::Level::ERR) << "Failed to initialize COM, error: " << hr;
            return EXIT_FAILURE;
        }

#ifdef DEBUG
        if (!AllocConsole())
        {
            Log(Log::Level::INFO) << "Attached to console";
        }
#endif

        if (!init())
        {
            return EXIT_FAILURE;
        }

        if (sharedEngine)
        {
            sharedEngine->start();
        }
        else
        {
            return EXIT_FAILURE;
        }

        input::InputWin* input = static_cast<input::InputWin*>(sharedEngine->getInput());
        WindowWin* window = static_cast<WindowWin*>(sharedEngine->getWindow());

        MSG msg;

        while (active)
        {
            executeAll();

            if (sharedEngine->draw())
            {
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
                            exit();
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
                        exit();
                        break;
                    }
                }

                input->update();
            }
            else
            {
                break;
            }
        }

        if (sharedEngine)
        {
            sharedEngine->stop();
        }

        return EXIT_SUCCESS;
    }

    void ApplicationWin::execute(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    void ApplicationWin::executeAll()
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

    bool ApplicationWin::openURL(const std::string& url)
    {
        wchar_t urlBuffer[256];
        MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, urlBuffer, 256);
        intptr_t result = reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", urlBuffer, nullptr, nullptr, SW_SHOWNORMAL));
        return result > 32;
    }
}
