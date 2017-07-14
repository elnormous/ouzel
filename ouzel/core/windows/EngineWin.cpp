// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include <cstdlib>
#include "EngineWin.h"
#include "WindowWin.h"
#include "input/windows/InputWin.h"
#include "utils/Log.h"

namespace ouzel
{
    EngineWin::EngineWin()
    {
        LPWSTR* argList;
        int nArgs;
        int i;

        argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);

        std::vector<std::string> args;

        if (argList)
        {
            for (i = 0; i < nArgs; i++)
            {
                char temporaryCString[256];
                WideCharToMultiByte(CP_UTF8, 0, argList[i], -1, temporaryCString, sizeof(temporaryCString), nullptr, nullptr);

                args.push_back(temporaryCString);
            }

            LocalFree(argList);
        }
    }

    EngineWin::~EngineWin()
    {
        //CoUninitialize();
    }

    int EngineWin::run()
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

        start();

        input::InputWin* inputWin = static_cast<input::InputWin*>(input.get());
        WindowWin* windowWin = static_cast<WindowWin*>(window.get());

        MSG msg;

        while (active)
        {
            executeAll();

            if (renderer->process())
            {
                std::set<HACCEL> accelerators = windowWin->getAccelerators();

                while (active)
                {
                    if (running)
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
                        if (TranslateAccelerator(windowWin->getNativeWindow(), accelerator, &msg))
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

                inputWin->update();
            }
            else
            {
                break;
            }
        }

        // must join the update thread before exiting
#if OUZEL_MULTITHREADED
        if (updateThread.joinable()) updateThread.join();
#endif

        return EXIT_SUCCESS;
    }

    void EngineWin::execute(const std::function<void(void)>& func)
    {
        std::lock_guard<std::mutex> lock(executeMutex);

        executeQueue.push(func);
    }

    void EngineWin::executeAll()
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

    bool EngineWin::openURL(const std::string& url)
    {
        wchar_t urlBuffer[256];
        MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, urlBuffer, 256);
        intptr_t result = reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", urlBuffer, nullptr, nullptr, SW_SHOWNORMAL));
        return result > 32;
    }
}
