// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
#include <Windows.h>
#include <cstdlib>
#include "EngineWin.hpp"
#include "NativeWindowWin.hpp"
#include "input/windows/InputSystemWin.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    EngineWin::EngineWin(int initArgc, LPWSTR* initArgv)
    {

        if (initArgv)
        {
            int bufferSize;
            std::vector<char> buffer;

            for (int i = 0; i < initArgc; ++i)
            {
                bufferSize = WideCharToMultiByte(CP_UTF8, 0, initArgv[i], -1, nullptr, 0, nullptr, nullptr);
                if (bufferSize == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                buffer.resize(bufferSize);
                if (WideCharToMultiByte(CP_UTF8, 0, initArgv[i], -1, buffer.data(), bufferSize, nullptr, nullptr) == 0)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                args.push_back(buffer.data());
            }
        }
    }

    EngineWin::~EngineWin()
    {
        //CoUninitialize();
    }

    static void translateMessage(HWND window, const std::set<HACCEL>& accelerators, MSG& message)
    {
        bool translate = true;

        for (HACCEL accelerator : accelerators)
        {
            if (TranslateAccelerator(window, accelerator, &message))
                translate = false;
        }

        if (translate)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    void EngineWin::run()
    {
        HRESULT hr;
        if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
            throw SystemError("Failed to initialize COM, error: " + std::to_string(hr));

#ifdef DEBUG
        if (!AllocConsole())
            engine->log(Log::Level::INFO) << "Attached to console";
#endif

        init();
        start();

        input::InputSystemWin* inputWin = static_cast<input::InputSystemWin*>(inputManager->getInputSystem());
        NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

        MSG message;

        while (active)
        {
            if (!paused)
            {
                if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
                {
                    translateMessage(windowWin->getNativeWindow(),
                                     windowWin->accelerators, message);

                    if (message.message == WM_QUIT)
                    {
                        exit();
                        break;
                    }
                }
            }
            else
            {
                BOOL ret = GetMessage(&message, nullptr, 0, 0);
                if (ret == 0)
                {
                    exit();
                    break;
                }
                else if (ret == -1)
                {
                    exit();
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get message");
                }
                else
                {
                    translateMessage(windowWin->getNativeWindow(),
                                     windowWin->accelerators, message);
                }
            }

            inputWin->update();
        }

        exit();
    }

    void EngineWin::executeOnMainThread(const std::function<void()>& func)
    {
        NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

        {
            std::unique_lock<std::mutex> lock(executeMutex);
            executeQueue.push(func);
        }

        PostMessage(windowWin->getNativeWindow(), WM_USER, 0, 0);
    }

    void EngineWin::executeAll()
    {
        std::function<void()> func;

        for (;;)
        {
            std::unique_lock<std::mutex> lock(executeMutex);

            if (executeQueue.empty())
                break;

            func = std::move(executeQueue.front());
            executeQueue.pop();
            lock.unlock();

            if (func) func();
        }
    }

    void EngineWin::openURL(const std::string& url)
    {
        int buferSize = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
        if (buferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> buffer(buferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, buffer.data(), buferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        intptr_t result = reinterpret_cast<intptr_t>(ShellExecuteW(nullptr, L"open", buffer.data(), nullptr, nullptr, SW_SHOWNORMAL));
        if (result <= 32)
            throw SystemError("Failed to execute open");
    }
}
