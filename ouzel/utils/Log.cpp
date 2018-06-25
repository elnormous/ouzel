// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include "core/Setup.h"
#include <iostream>
#include <string>

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <sys/syslog.h>
#endif

#if OUZEL_PLATFORM_WINDOWS
#include <Windows.h>
#include <strsafe.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#if OUZEL_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

#include "Log.hpp"

namespace ouzel
{
#ifdef DEBUG
    Log::Level Log::threshold = Log::Level::ALL;
#else
    Log::Level Log::threshold = Log::Level::INFO;

#endif

    Log::~Log()
    {
        if (!s.empty())
        {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN
            switch (level)
            {
                case Level::ERR:
                case Level::WARN:
                    std::cerr << s << std::endl;
                    break;
                case Level::INFO:
                case Level::ALL:
                    std::cout << s << std::endl;
                    break;
                default: break;
            }
#elif OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            int priority = 0;
            switch (level)
            {
                case Level::ERR: priority = LOG_ERR; break;
                case Level::WARN: priority = LOG_WARNING; break;
                case Level::INFO: priority = LOG_INFO; break;
                case Level::ALL: priority = LOG_DEBUG; break;
                default: break;
            }
            syslog(priority, "%s", s.c_str());
#elif OUZEL_PLATFORM_WINDOWS
            std::vector<wchar_t> szBuffer(s.length() + 1 + 1); // for the newline and the terminating char
            if (MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, szBuffer.data(), static_cast<int>(szBuffer.size())) == 0)
                return;

            StringCchCatW(szBuffer.data(), szBuffer.size(), L"\n");
            OutputDebugStringW(szBuffer.data());

#if DEBUG
            HANDLE handle = 0;
            switch (level)
            {
            case Level::ERR:
            case Level::WARN:
                handle = GetStdHandle(STD_ERROR_HANDLE);
                break;
            case Level::INFO:
            case Level::ALL:
                handle = GetStdHandle(STD_OUTPUT_HANDLE);
                break;
            default: break;
            }

            if (handle)
            {
                DWORD bytesWritten;
                WriteConsoleW(handle, szBuffer.data(), static_cast<DWORD>(wcslen(szBuffer.data())), &bytesWritten, nullptr);
            }
#endif // #if DEBUG

#elif OUZEL_PLATFORM_ANDROID
            int priority = 0;
            switch (level)
            {
                case Level::ERR: priority = ANDROID_LOG_ERROR; break;
                case Level::WARN: priority = ANDROID_LOG_WARN; break;
                case Level::INFO: priority = ANDROID_LOG_INFO; break;
                case Level::ALL: priority = ANDROID_LOG_DEBUG; break;
                default: break;
            }
            __android_log_print(priority, "Ouzel", "%s", s.c_str());
#elif OUZEL_PLATFORM_EMSCRIPTEN
            int flags = EM_LOG_CONSOLE;
            if (level == Level::ERR) flags |= EM_LOG_ERROR;
            else if (level == Level::WARN) flags |= EM_LOG_WARN;
            emscripten_log(flags, "%s", s.c_str());
#endif
            s.clear();
        }
    }
}
