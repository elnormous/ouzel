// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
    Log::~Log()
    {
        if (!s.empty())
        {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
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
            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
            if (bufferSize == 0)
                return;

            ++bufferSize; // for the newline
            std::vector<WCHAR> buffer(bufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer.data(), bufferSize) == 0)
                return;

            StringCchCatW(buffer.data(), buffer.size(), L"\n");
            OutputDebugStringW(buffer.data());

#  if DEBUG
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
                WriteConsoleW(handle, buffer.data(), static_cast<DWORD>(wcslen(buffer.data())), &bytesWritten, nullptr);
            }
#  endif

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
        }
    }
}
