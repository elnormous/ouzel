// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#include <sys/time.h>
#endif

#ifdef OUZEL_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "Utils.h"

namespace ouzel
{
    void log(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        
        vprintf(format, list);
        printf("\n");
        
        va_end(list);
    }
    
    uint64_t getCurrentMicroSeconds()
    {
#if defined(OUZEL_PLATFORM_OSX)
        struct timeval currentTime;
        
        gettimeofday(&currentTime, NULL);
        return currentTime.tv_sec * 1000000L + currentTime.tv_usec;
#elif defined(OUZEL_PLATFORM_WINDOWS)
        LARGE_INTEGER li;
        if (!QueryPerformanceFrequency(&li))
        {
            log("Failed to query frequency");
            return 0;
        }
        uint64_t frequency = li.QuadPart / 1000000L;
        
        QueryPerformanceCounter(&li);
        
        return li.QuadPart / frequency;
#else
        return 0;
#endif
    }
}
