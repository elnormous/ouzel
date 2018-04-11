// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(_WIN32) // Windows
#define OUZEL_PLATFORM_WINDOWS 1
#define OUZEL_SUPPORTS_DIRECT3D 1
#define OUZEL_SUPPORTS_DIRECT3D11 1
#define OUZEL_SUPPORTS_OPENGL 1
#define OUZEL_OPENGL_INTERFACE_WGL 1
#define OUZEL_SUPPORTS_DIRECTSOUND 1

#if defined(_MSC_VER) // Visual Studio
#define OUZEL_SUPPORTS_XAUDIO2 1
#endif

#define OUZEL_MULTITHREADED 1

#if defined(_M_X64)
#define OUZEL_64BITS 1
#elif defined(_M_IX86)
#define OUZEL_32BITS 1
#endif

// #if defined(_WIN32)
#elif defined(__APPLE__) // macOS, iOS, tvOS
#include <TargetConditionals.h>
#define OUZEL_SUPPORTS_OPENGL 1
#define OUZEL_SUPPORTS_OPENAL 1
#define OUZEL_SUPPORTS_COREAUDIO 1

#if TARGET_OS_IOS
#define OUZEL_PLATFORM_IOS 1
#define OUZEL_SUPPORTS_OPENGLES 1
#define OUZEL_OPENGL_INTERFACE_EAGL 1
#if !TARGET_OS_SIMULATOR
#define OUZEL_SUPPORTS_METAL 1
#endif
// #if TARGET_OS_IOS
#elif TARGET_OS_TV
#define OUZEL_PLATFORM_TVOS 1
#define OUZEL_SUPPORTS_OPENGLES 1
#define OUZEL_OPENGL_INTERFACE_EAGL 1
#if !TARGET_OS_SIMULATOR
#define OUZEL_SUPPORTS_METAL 1
#endif
// #elif TARGET_OS_TV
#elif TARGET_OS_MAC
#define OUZEL_PLATFORM_MACOS 1
#define OUZEL_OPENGL_INTERFACE_CGL 1
#if !TARGET_OS_SIMULATOR
#define OUZEL_SUPPORTS_METAL 1
#endif
#endif // #elif TARGET_OS_MAC

#define OUZEL_MULTITHREADED 1

#if defined(__x86_64__)
#define OUZEL_64BITS 1
#elif defined(__i386__)
#define OUZEL_32BITS 1
#elif defined(__arm64__)
#define OUZEL_64BITS 1
#elif defined(__arm__)
#define OUZEL_32BITS 1
#endif

#if defined(__ARM_NEON__)
#define OUZEL_SUPPORTS_NEON 1
#endif // #if defined(__ARM_NEON__)

// #if defined(__APPLE__)
#elif defined(__ANDROID__) // Android
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
#elif defined(__arm64__) || defined(__aarch64__)
#define OUZEL_64BITS 1
#elif defined(__arm__)
#define OUZEL_32BITS 1
#endif

#if defined(__ARM_NEON__)
#define OUZEL_SUPPORTS_NEON 1
#if defined(__arm__) // NEON support must be checked on Android for 32-bit platforms
#define OUZEL_SUPPORTS_NEON_CHECK 1
#endif // #if defined(__arm__)
#endif // #if defined(__ARM_NEON__)

// #if defined(__ANDROID__)
#elif defined(__linux__) // Linux

#if defined(__arm64__) || defined(__aarch64__) || defined(__arm__) // Raspbian
#define OUZEL_PLATFORM_RASPBIAN 1
#define OUZEL_SUPPORTS_OPENGL 1
#define OUZEL_SUPPORTS_OPENGLES 1
#define OUZEL_OPENGL_INTERFACE_EGL 1
#define OUZEL_SUPPORTS_OPENAL 1
#define OUZEL_SUPPORTS_ALSA 1
#define OUZEL_MULTITHREADED 1

#if defined(__arm64__) || defined(__aarch64__)
#define OUZEL_64BITS 1
#elif defined(__arm__)
#define OUZEL_32BITS 1
#endif

#if defined(__ARM_NEON__)
#define OUZEL_SUPPORTS_NEON 1
#endif // #if defined(__ARM_NEON__)

#else // #if defined(__arm64__) || defined(__aarch64__)

#define OUZEL_PLATFORM_LINUX 1
#define OUZEL_SUPPORTS_OPENGL 1
#define OUZEL_OPENGL_INTERFACE_GLX 1
#define OUZEL_SUPPORTS_OPENAL 1
#define OUZEL_SUPPORTS_ALSA 1
#define OUZEL_MULTITHREADED 1
#define OUZEL_SUPPORTS_X11 1

#if defined(__x86_64__)
#define OUZEL_64BITS 1
#elif defined(__i386__)
#define OUZEL_32BITS 1
#endif

#endif

// #if defined(__linux__)
#elif defined(EMSCRIPTEN) // Emscripten
#define OUZEL_PLATFORM_EMSCRIPTEN 1
#define OUZEL_SUPPORTS_OPENGL 1
#define OUZEL_SUPPORTS_OPENGLES 1
#define OUZEL_OPENGL_INTERFACE_EGL 1
#define OUZEL_SUPPORTS_OPENAL 1

#endif // #if defined(EMSCRIPTEN)

#if defined(__SSE__)
#define OUZEL_SUPPORTS_SSE 1
#endif
