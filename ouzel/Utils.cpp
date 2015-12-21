// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#include <sys/time.h>
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
    
    long getCurrentMicroSeconds()
    {
#ifdef OUZEL_PLATFORM_OSX
        struct timeval currentTime;
        
        gettimeofday(&currentTime, NULL);
        return currentTime.tv_sec * 1000000L + currentTime.tv_usec;
#else
        return 0.0; // TODO: implement for Windows
#endif
    }
}
