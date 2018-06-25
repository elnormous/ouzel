// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Windows.h>
#include <cstdlib>
#include "EngineWin.hpp"
#include "WindowResourceWin.hpp"
#include "input/windows/InputManagerWin.hpp"
#include "thread/Lock.hpp"
#include "utils/Errors.hpp"
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
    EngineWin::EngineWin(int initArgc, LPWSTR* initArgv)
    {
        if (initArgv)
        {
            char temporaryCString[256];
            for (int i = 0; i < initArgc; ++i)
            {
                WideCharToMultiByte(CP_UTF8, 0, initArgv[i], -1, temporaryCString, sizeof(temporaryCString), nullptr, nullptr);

                args.push_back(temporaryCString);
            }
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
                translate = false;
        }

        if (translate)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void EngineWin::run()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
            throw SystemError("Failed to initialize COM, error: " + std::to_string(hr));

#ifdef DEBUG
        if (!AllocConsole())
            Log(Log::Level::INFO) << "Attached to console";
#endif

        init();
        start();

        input::InputManagerWin* inputWin = static_cast<input::InputManagerWin*>(inputManager.get());
        WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(window.getResource());

        MSG msg;

        while (active)
        {
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
                    throw SystemError("Failed to get message");
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
    }

    void EngineWin::executeOnMainThread(const std::function<void(void)>& func)
    {
        WindowResourceWin* windowWin = static_cast<WindowResourceWin*>(window.getResource());

        Lock lock(executeMutex);

        executeQueue.push(func);

        PostMessage(windowWin->getNativeWindow(), WM_USER, 0, 0);
    }

    void EngineWin::executeAll()
    {
        std::function<void(void)> func;

        for (;;)
        {
            {
                Lock lock(executeMutex);

                if (executeQueue.empty())
                    break;

                func = std::move(executeQueue.front());
                executeQueue.pop();
            }

            if (func) func();
        }
    }

    void EngineWin::openURL(const std::string& url)
    {
        wchar_t urlBuffer[256];
        if (MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, urlBuffer, 256) == 0)
            throw SystemError("Failed to convert UTF-8 to wide char");

        intptr_t result = reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", urlBuffer, nullptr, nullptr, SW_SHOWNORMAL));
        if (result <= 32)
            throw SystemError("Failed to execute open");
    }
}
