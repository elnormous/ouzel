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

    static void translateMessage(HWND window, const std::set<HACCEL>& accelerators, MSG& msg)
    {
        bool translate = true;

        for (HACCEL accelerator : accelerators)
        {
            if (TranslateAccelerator(window, accelerator, &msg))
            {
                translate = false;
            }
        }

        if (translate)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
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

            if (running)
            {
                if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
                {
                    translateMessage(windowWin->getNativeWindow(),
                                     windowWin->accelerators, msg);

                    if (msg.message == WM_QUIT)
                    {
                        exit();
                        break;
                    }
                }
            }
            else
            {
                BOOL ret = GetMessage(&msg, nullptr, 0, 0);
                if (ret == 0)
                {
                    exit();
                    break;
                }
                else if (ret == -1)
                {
                    exit();
                    Log(Log::Level::ERR) << "Failed to get message";
                    return EXIT_FAILURE;
                }
                else
                {
                    translateMessage(windowWin->getNativeWindow(),
                                     windowWin->accelerators, msg);
                }
            }

            inputWin->update();
        }

        return EXIT_SUCCESS;
    }

    void EngineWin::executeOnMainThread(const std::function<void(void)>& func)
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
        if (MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, urlBuffer, 256) == 0)
        {
            Log(Log::Level::ERR) << "Failed to convert UTF-8 to wide char";
            return false;
        }

        intptr_t result = reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", urlBuffer, nullptr, nullptr, SW_SHOWNORMAL));
        return result > 32;
    }
}
