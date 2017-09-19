// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Config.h"

#if defined(_WIN32)
    #define OUZEL_PLATFORM_WINDOWS 1
    #define OUZEL_SUPPORTS_DIRECT3D 1
    #define OUZEL_SUPPORTS_DIRECT3D11 1
    #define OUZEL_SUPPORTS_OPENGL 1
    #define OUZEL_OPENGL_INTERFACE_WGL 1
    #define OUZEL_SUPPORTS_DIRECTSOUND 1
    #if defined(_MSC_VER)
        #define OUZEL_SUPPORTS_XAUDIO2 1
    #endif
    #define OUZEL_MULTITHREADED 1

    #if defined(_M_X64)
        #define OUZEL_64BITS 1
    #elif defined(_M_IX86)
        #define OUZEL_32BITS 1
    #endif
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #define OUZEL_SUPPORTS_OPENGL 1
    #define OUZEL_SUPPORTS_METAL 1
    #define OUZEL_SUPPORTS_OPENAL 1
    #define OUZEL_SUPPORTS_COREAUDIO 1

    #if TARGET_OS_IOS
        #define OUZEL_PLATFORM_IOS 1
        #define OUZEL_SUPPORTS_OPENGLES 1
        #define OUZEL_OPENGL_INTERFACE_EAGL 1
    #elif TARGET_OS_TV
        #define OUZEL_PLATFORM_TVOS 1
        #define OUZEL_SUPPORTS_OPENGLES 1
        #define OUZEL_OPENGL_INTERFACE_EAGL 1

    #elif TARGET_OS_MAC
        #define OUZEL_PLATFORM_MACOS 1
        #define OUZEL_OPENGL_INTERFACE_CGL 1
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
    #define OUZEL_SUPPORTS_OPENGL 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_OPENGL_INTERFACE_EGL 1
    #define OUZEL_SUPPORTS_OPENSL 1
    #define OUZEL_SUPPORTS_OPENSLES 1
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
    #define OUZEL_SUPPORTS_OPENGL 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_OPENGL_INTERFACE_EGL 1
    #define OUZEL_SUPPORTS_OPENAL 1
    #define OUZEL_SUPPORTS_ALSA 1
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
    #define OUZEL_SUPPORTS_OPENGL 1
    #if defined(__arm__) || defined(__aarch64__)
        #define OUZEL_SUPPORTS_OPENGLES 1
    #endif
    #define OUZEL_OPENGL_INTERFACE_GLX 1
    #define OUZEL_SUPPORTS_OPENAL 1
    #define OUZEL_SUPPORTS_ALSA 1
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
    #define OUZEL_SUPPORTS_OPENGL 1
    #define OUZEL_SUPPORTS_OPENGLES 1
    #define OUZEL_OPENGL_INTERFACE_EGL 1
    #define OUZEL_SUPPORTS_OPENAL 1
#endif

#if defined(__SSE__)
    #define OUZEL_SUPPORTS_SSE 1
#endif

// OpenGL
#ifndef OUZEL_ENABLE_OPENGL
#define OUZEL_ENABLE_OPENGL 1
#endif
#if OUZEL_SUPPORTS_OPENGL && OUZEL_ENABLE_OPENGL
#define OUZEL_COMPILE_OPENGL 1
#endif

// Direct3D 11
#ifndef OUZEL_ENABLE_DIRECT3D11
#define OUZEL_ENABLE_DIRECT3D11 1
#endif
#if OUZEL_SUPPORTS_DIRECT3D11 && OUZEL_ENABLE_DIRECT3D11
#define OUZEL_COMPILE_DIRECT3D11 1
#endif

// Metal
#ifndef OUZEL_ENABLE_METAL
#define OUZEL_ENABLE_METAL 1
#endif
#if OUZEL_SUPPORTS_METAL && OUZEL_ENABLE_METAL
#define OUZEL_COMPILE_METAL 1
#endif

// OpenAL
#ifndef OUZEL_ENABLE_OPENAL
#define OUZEL_ENABLE_OPENAL 1
#endif
#if OUZEL_SUPPORTS_OPENAL && OUZEL_ENABLE_OPENAL
#define OUZEL_COMPILE_OPENAL 1
#endif

// DirectSound
#ifndef OUZEL_ENABLE_DIRECTSOUND
#define OUZEL_ENABLE_DIRECTSOUND 1
#endif
#if OUZEL_SUPPORTS_DIRECTSOUND && OUZEL_ENABLE_DIRECTSOUND
#define OUZEL_COMPILE_DIRECTSOUND 1
#endif

// XAudio 2
#ifndef OUZEL_ENABLE_XAUDIO2
#define OUZEL_ENABLE_XAUDIO2 1
#endif
#if OUZEL_SUPPORTS_XAUDIO2 && OUZEL_ENABLE_XAUDIO2
#define OUZEL_COMPILE_XAUDIO2 1
#endif

// OpenSL
#ifndef OUZEL_ENABLE_OPENSL
#define OUZEL_ENABLE_OPENSL 1
#endif
#if OUZEL_SUPPORTS_OPENSL && OUZEL_ENABLE_OPENSL
#define OUZEL_COMPILE_OPENSL 1
#endif

// CoreAudio
#ifndef OUZEL_ENABLE_COREAUDIO
#define OUZEL_ENABLE_COREAUDIO 1
#endif
#if OUZEL_SUPPORTS_COREAUDIO && OUZEL_ENABLE_COREAUDIO
#define OUZEL_COMPILE_COREAUDIO 1
#endif

// ALSA
#ifndef OUZEL_ENABLE_ALSA
#define OUZEL_ENABLE_ALSA 1
#endif
#if OUZEL_SUPPORTS_ALSA && OUZEL_ENABLE_ALSA
#define OUZEL_COMPILE_ALSA 1
#endif
