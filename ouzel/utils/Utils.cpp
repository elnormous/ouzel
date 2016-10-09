// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdarg>
#include <random>
#include "core/CompileConfig.h"

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

#include "Utils.h"

namespace ouzel
{
#if OUZEL_PLATFORM_ANDROID && OUZEL_SUPPORTS_NEON_CHECK
    AnrdoidNEONChecker anrdoidNEONChecker;
#endif

#ifdef DEBUG
    static LogLevel logLevel = LOG_LEVEL_VERBOSE;
#else
    static LogLevel logLevel = LOG_LEVEL_INFO;
#endif

    void setLogLevel(LogLevel level)
    {
        logLevel = level;
    }

    static const std::function<void(LogLevel, const std::string&)> defaultLogCallback = [] (LogLevel level, const std::string& str) {
#if OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX || OUZEL_PLATFORM_RASPBIAN || OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        FILE* f = nullptr;
        switch (level)
        {
            case LOG_LEVEL_ERROR:
            case LOG_LEVEL_WARNING:
                f = stderr;
                break;
            case LOG_LEVEL_INFO:
            case LOG_LEVEL_VERBOSE:
                f = stdout;
                break;
        }
        fputs(str.c_str(), f);
        fputc('\n', f);
    #if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
        int prio = 0;
        switch (level)
        {
            case LOG_LEVEL_ERROR: prio = LOG_ERR; break;
            case LOG_LEVEL_WARNING: prio = LOG_WARNING; break;
            case LOG_LEVEL_INFO: prio = LOG_INFO; break;
            case LOG_LEVEL_VERBOSE: prio = LOG_DEBUG; break;
        }
        syslog(prio, "%s", str.c_str());
    #endif
#elif OUZEL_PLATFORM_WINDOWS
        OUZEL_UNUSED(level);
        wchar_t szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuffer, MAX_PATH);
        StringCchCat(szBuffer, sizeof(szBuffer), L"\n");
        OutputDebugString(szBuffer);
#elif OUZEL_PLATFORM_ANDROID
        int prio = 0;
        switch (level)
        {
            case LOG_LEVEL_ERROR: prio = ANDROID_LOG_ERROR; break;
            case LOG_LEVEL_WARNING: prio = ANDROID_LOG_WARN; break;
            case LOG_LEVEL_INFO: prio = ANDROID_LOG_INFO; break;
            case LOG_LEVEL_VERBOSE: prio = ANDROID_LOG_DEBUG; break;
        }
        __android_log_print(prio, "Ouzel", "%s", str.c_str());
#elif OUZEL_PLATFORM_EMSCRIPTEN
        int flags = EM_LOG_CONSOLE;
        if (level == LOG_LEVEL_ERROR) flags |= EM_LOG_ERROR;
        else if (level == LOG_LEVEL_WARNING) flags |= EM_LOG_WARN;
        emscripten_log(flags, "%s", str.c_str());
#endif
    };

    std::function<void(LogLevel, const std::string&)> logCallback = defaultLogCallback;

    void setLogCallback(const std::function<void(LogLevel, const std::string&)>& callback)
    {
        logCallback = callback;
    }

    static char TEMP_BUFFER[1024];
    void log(LogLevel level, const char* format, ...)
    {
        if (level <= logLevel)
        {
            va_list list;
            va_start(list, format);

            vsprintf(TEMP_BUFFER, format, list);

            va_end(list);

            logCallback(level, TEMP_BUFFER);
        }
    }

    static std::mt19937 engine(std::random_device{}());

    uint32_t random(uint32_t min, uint32_t max)
    {
        return std::uniform_int_distribution<uint32_t>{min, max}(engine);
    }

    float randomf(float min, float max)
    {
        float diff = max - min;

        float rand = static_cast<float>(engine()) / engine.max();

        return rand * diff + min;
    }
}
