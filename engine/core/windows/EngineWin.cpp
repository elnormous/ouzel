// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <system_error>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <shellapi.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "EngineWin.hpp"
#include "NativeWindowWin.hpp"
#include "../../input/windows/InputSystemWin.hpp"
#include "../../utils/Log.hpp"

namespace ouzel
{
    namespace
    {
        class ShellExecuteErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept final
            {
                return "ShellExecute";
            }

            std::string message(int condition) const final
            {
                switch (condition)
                {
                    case 0: return "Out of memory";
                    case ERROR_FILE_NOT_FOUND: return "ERROR_FILE_NOT_FOUND";
                    case ERROR_PATH_NOT_FOUND: return "ERROR_PATH_NOT_FOUND";
                    case ERROR_BAD_FORMAT: return "ERROR_BAD_FORMAT";
                    case SE_ERR_ACCESSDENIED: return "SE_ERR_ACCESSDENIED";
                    case SE_ERR_ASSOCINCOMPLETE: return "SE_ERR_ASSOCINCOMPLETE";
                    case SE_ERR_DDEBUSY: return "SE_ERR_DDEBUSY";
                    case SE_ERR_DDEFAIL: return "SE_ERR_DDEFAIL";
                    case SE_ERR_DDETIMEOUT: return "SE_ERR_DDETIMEOUT";
                    case SE_ERR_DLLNOTFOUND: return "SE_ERR_DLLNOTFOUND";
                    case SE_ERR_NOASSOC: return "SE_ERR_NOASSOC";
                    case SE_ERR_OOM: return "SE_ERR_OOM";
                    case SE_ERR_SHARE: return "SE_ERR_SHARE";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const ShellExecuteErrorCategory shellExecuteErrorCategory {};
    }

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

        HRESULT hr;
        if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
            throw std::system_error(hr, std::system_category(), "Failed to initialize COM");

#ifdef DEBUG
        if (!AllocConsole())
            log(Log::Level::info) << "Attached to console";
#endif
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
                BOOL ret;
                if ((ret = GetMessage(&message, nullptr, 0, 0)) == -1)
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get message");

                if (ret == 0)
                {
                    exit();
                    break;
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

    void EngineWin::runOnMainThread(const std::function<void()>& func)
    {
        NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(window->getNativeWindow());

        std::unique_lock<std::mutex> lock(executeMutex);
        executeQueue.push(func);
        lock.unlock();

        if (!PostMessage(windowWin->getNativeWindow(), WM_USER, 0, 0))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to post message");

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

    void EngineWin::openUrl(const std::string& url)
    {
        const int buferSize = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
        if (buferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        auto buffer = std::make_unique<WCHAR[]>(buferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, buffer.get(), buferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        // Result of the ShellExecuteW can be cast only to an int (https://docs.microsoft.com/en-us/windows/desktop/api/shellapi/nf-shellapi-shellexecutew)
        HINSTANCE result = ShellExecuteW(nullptr, L"open", buffer.get(), nullptr, nullptr, SW_SHOWNORMAL);
        int status;
        memcpy(&status, &result, sizeof(status));
        if (status <= 32)
            throw std::system_error(status, shellExecuteErrorCategory, "Failed to execute open");
    }
}
