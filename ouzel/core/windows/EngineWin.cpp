// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include <cstdlib>
#include "EngineWin.hpp"
#include "WindowWin.hpp"
#include "input/windows/InputWin.hpp"
#include "utils/Log.hpp"

const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)

namespace ouzel
{
    EngineWin::EngineWin()
    {
        int nArgs;
        LPWSTR* argList = CommandLineToArgvW(GetCommandLineW(), &nArgs);

        if (argList)
        {
            char temporaryCString[256];
            for (int i = 0; i < nArgs; i++)
            {
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

            if (!paused)
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

        exit();

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

    bool EngineWin::setCurrentThreadName(const std::string& name)
    {
#ifndef __GNUC__ // clang and gcc do not support SEH exceptions
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = static_cast<DWORD>(-1);
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        return true;
#else
        return false;
#endif
    }
}
