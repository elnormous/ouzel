// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"
#include <iostream>
#include <string>

#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
#include <sys/syslog.h>
#endif

#if OUZEL_PLATFORM_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <strsafe.h>
#endif

#if OUZEL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#if OUZEL_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

#include "Log.h"

namespace ouzel
{
#ifndef DEBUG
    Log::Level Log::threshold = Log::Level::INFO;
#else
    Log::Level Log::threshold = Log::Level::ALL;
#endif

    Log::~Log()
    {
        if (level <= threshold)
        {
            switch (level)
            {
            case Level::ERR:
            case Level::WARN:
                std::cerr << s.str() << std::endl;
                break;
            case Level::INFO:
            case Level::ALL:
                std::cout << s.str() << std::endl;
                break;
            default: break;
            }
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
            int priority = 0;
            switch (level)
            {
            case Level::ERR: priority = LOG_ERR; break;
            case Level::WARN: priority = LOG_WARNING; break;
            case Level::INFO: priority = LOG_INFO; break;
            case Level::ALL: priority = LOG_DEBUG; break;
            default: break;
            }
            syslog(priority, "%s", s.str().c_str());
#endif

#if OUZEL_PLATFORM_WINDOWS
            wchar_t szBuffer[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, s.str().c_str(), -1, szBuffer, MAX_PATH);
            StringCchCatW(szBuffer, sizeof(szBuffer), L"\n");
            OutputDebugStringW(szBuffer);
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
            __android_log_print(priority, "Ouzel", "%s", s.str().c_str());
#elif OUZEL_PLATFORM_EMSCRIPTEN
            int flags = EM_LOG_CONSOLE;
            if (level == Level::ERR) flags |= EM_LOG_ERROR;
            else if (level == Level::WARN) flags |= EM_LOG_WARN;
            emscripten_log(flags, "%s", s.str().c_str());
#endif
        }
    }
}
