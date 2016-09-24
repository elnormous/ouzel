// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define OUZEL_PLATFORM_WINDOWS 1
    #define OUZEL_SUPPORTS_DIRECT3D 1
    #define OUZEL_SUPPORTS_DIRECT3D11 1
    #define OUZEL_SUPPORTS_XAUDIO2 1
    #define OUZEL_MULTITHREADED 1

    #if defined(_M_X64)
        #define OUZEL_64BITS 1
    #elif defined(_M_IX86)
        #define OUZEL_32BITS 1
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define OUZEL_PLATFORM_IOS 1
        #define OUZEL_SUPPORTS_OPENGLES 1
        #define OUZEL_SUPPORTS_OPENGLES3 1
        #define OUZEL_SUPPORTS_METAL 1
        #define OUZEL_SUPPORTS_OPENAL 1
    #elif TARGET_OS_TV
        #define OUZEL_PLATFORM_TVOS 1
        #define OUZEL_SUPPORTS_OPENGLES 1
        #define OUZEL_SUPPORTS_OPENGLES3 1
        #define OUZEL_SUPPORTS_METAL 1
        #define OUZEL_SUPPORTS_OPENAL 1
    #elif TARGET_OS_MAC
        #define OUZEL_PLATFORM_MACOS 1
        #define OUZEL_SUPPORTS_OPENGL 1
        #define OUZEL_SUPPORTS_OPENGL3 1
        #define OUZEL_SUPPORTS_METAL 1
        #define OUZEL_SUPPORTS_OPENAL 1
    #endif

    #define OUZEL_MULTITHREADED 1

    #if defined(__x86_64__)
        #define OUZEL_64BITS 1
    #elif defined(__i386__)
        #define OUZEL_32BITS 1
    #elif defined(__ARM_NEON__)
        #if defined(__arm64__)
            #define OUZEL_SUPPORTS_NEON64 1
            #define OUZEL_64BITS 1
        #elif defined(__arm__)
            #define OUZEL_SUPPORTS_NEON 1
            #define OUZEL_32BITS 1
        #endif
    #endif

#elif defined(__ANDROID__)
    #define OUZEL_PLATFORM_ANDROID 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_SUPPORTS_OPENGLES3 1
    #define OUZEL_SUPPORTS_OPENSL 1
    #define OUZEL_MULTITHREADED 1

    #if defined(__x86_64__)
        #define OUZEL_64BITS 1
    #elif defined(__i386__)
        #define OUZEL_32BITS 1
    #elif defined(__ARM_NEON__)
        #if defined(__arm64__) || defined(__aarch64__)
            #define OUZEL_SUPPORTS_NEON64 1
            #define OUZEL_64BITS 1
        #elif defined(__arm__)
            #define OUZEL_SUPPORTS_NEON 1
            #define OUZEL_SUPPORTS_NEON_CHECK 1
            #define OUZEL_32BITS 1
        #endif
    #endif
#elif defined(RASPBIAN)
    #define OUZEL_PLATFORM_RASPBIAN 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_SUPPORTS_OPENGLES3 1
    #define OUZEL_SUPPORTS_OPENAL 1
    #define OUZEL_MULTITHREADED 1

    #if defined(__x86_64__)
        #define OUZEL_64BITS 1
    #elif defined(__i386__)
        #define OUZEL_32BITS 1
    #elif defined(__ARM_NEON__)
        #if defined(__arm64__) || defined(__aarch64__)
            #define OUZEL_SUPPORTS_NEON64 1
            #define OUZEL_64BITS 1
        #else defined(__arm__)
            #define OUZEL_SUPPORTS_NEON 1
            #define OUZEL_32BITS 1
        #endif
    #endif
#elif defined(__linux__)
    #define OUZEL_PLATFORM_LINUX 1
    #if defined(__i386__) || defined(__x86_64__)
        #define OUZEL_SUPPORTS_OPENGL 1
        #define OUZEL_SUPPORTS_OPENGL3 1
    #elif defined(__arm__) || defined(__aarch64__)
        #define OUZEL_SUPPORTS_OPENGLES 1
        #define OUZEL_SUPPORTS_OPENGLES3 1
    #endif
    #define OUZEL_SUPPORTS_OPENAL 1
    #define OUZEL_MULTITHREADED 1

    #if defined(__x86_64__)
        #define OUZEL_64BITS 1
    #elif defined(__i386__)
        #define OUZEL_32BITS 1
    #elif defined(__ARM_NEON__)
        #if defined(__arm64__) || defined(__aarch64__)
            #define OUZEL_SUPPORTS_NEON64 1
            #define OUZEL_64BITS 1
        #else defined(__arm__)
            #define OUZEL_SUPPORTS_NEON 1
            #define OUZEL_32BITS 1
        #endif
    #endif
#elif defined(EMSCRIPTEN)
    #define OUZEL_PLATFORM_EMSCRIPTEN 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_SUPPORTS_OPENAL 1
#endif

#if defined(__SSE__)
    #define OUZEL_SUPPORTS_SSE 1
#endif
