// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"

#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include <sys/time.h>
#endif

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
#include <sys/syslog.h>
#endif

#ifdef OUZEL_PLATFORM_WINDOWS
#include <windows.h>
#include <strsafe.h>
#endif

#ifdef OUZEL_PLATFORM_ANDROID
#include <android/log.h>
#endif

#include "Utils.h"

namespace ouzel
{
    char TEMP_BUFFER[65536];

    void log(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        
        vsprintf(TEMP_BUFFER, format, list);
        
        va_end(list);
        
#if defined(OUZEL_PLATFORM_OSX)
        printf("%s\n", TEMP_BUFFER);
#elif defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
        syslog(LOG_WARNING, "%s", TEMP_BUFFER);
        printf("%s\n", TEMP_BUFFER);
#elif defined(OUZEL_PLATFORM_WINDOWS)
        wchar_t szBuffer[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, TEMP_BUFFER, -1, szBuffer, MAX_PATH);
        StringCchCat(szBuffer, sizeof(szBuffer), L"\n");
        OutputDebugString(szBuffer);
#elif defined(OUZEL_PLATFORM_ANDROID)
        __android_log_print(ANDROID_LOG_DEBUG, "Ouzel", "%s", TEMP_BUFFER);
#endif
    }
    
    uint64_t getCurrentMicroSeconds()
    {
#if defined(OUZEL_PLATFORM_OSX) || defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS) || defined(OUZEL_PLATFORM_ANDROID)
        struct timeval currentTime;
        
        gettimeofday(&currentTime, NULL);
        return currentTime.tv_sec * 1000000L + currentTime.tv_usec;
#elif defined(OUZEL_PLATFORM_WINDOWS)
        
        static uint64_t frequency = 0;
        LARGE_INTEGER li;
        
        if (!frequency)
        {
            if (!QueryPerformanceFrequency(&li))
            {
                log("Failed to query frequency");
                return 0;
            }
            frequency = li.QuadPart / 1000000L;
        }
        
        QueryPerformanceCounter(&li);
        
        return li.QuadPart / frequency;
#else
        return 0;
#endif
    }
}
