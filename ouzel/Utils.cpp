// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "CompileConfig.h"

#ifdef OUZEL_PLATFORM_OSX
#include <OpenGL/gl3.h>
#endif

#include <sys/time.h>
#include <CoreGraphics/CoreGraphics.h>
#include "Utils.h"

namespace ouzel
{
    bool checkOpenGLErrors()
    {
        bool error = false;
        
        while (GLenum error = glGetError() != GL_NO_ERROR)
        {
            printf("OpenGL error: %d (%x)\n", error, error);
            
            error = true;
        }
        
        return error;
    }

    std::string getResourcePath(const std::string& filename)
    {
        CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef urlString = CFURLCopyPath(appUrlRef);
        
        char temporaryCString[1024] = "";
        
        CFStringGetCString(urlString, temporaryCString, sizeof(temporaryCString), kCFStringEncodingUTF8);
        
        CFRelease(appUrlRef);
        CFRelease(urlString);
        
        return std::string(temporaryCString) + "Contents/Resources/" + filename;
    }

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
