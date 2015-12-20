// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

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
}
