// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PLATFORM_H
#define OUZEL_PLATFORM_H

#if defined(_WIN32) // Windows
#  define OUZEL_PLATFORM_WINDOWS 1
#  define OUZEL_SUPPORTS_DIRECT3D 1
#  define OUZEL_SUPPORTS_DIRECT3D11 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_OPENGL_INTERFACE_WGL 1
#  define OUZEL_SUPPORTS_DIRECTSOUND 1
#  define OUZEL_SUPPORTS_WASAPI 1

#  if defined(_MSC_VER) // Visual Studio
#    define OUZEL_SUPPORTS_XAUDIO2 1
#  endif

#elif defined(__APPLE__) // macOS, iOS, tvOS
#  include <TargetConditionals.h>
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENAL 1
#  define OUZEL_SUPPORTS_COREAUDIO 1

#  if TARGET_OS_WATCH // watchOS
#    error "Apple Watch is not supported"
#  elif TARGET_OS_IOS // iOS
#    define OUZEL_PLATFORM_IOS 1
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EAGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_TV // tvOS
#    define OUZEL_PLATFORM_TVOS 1
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EAGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_MAC // any other Apple OS (check this last because it is defined for all Apple platforms)
#    define OUZEL_PLATFORM_MACOS 1
#    define OUZEL_OPENGL_INTERFACE_CGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  endif

#elif defined(__ANDROID__) // Android (check this before Linux because __linux__ is also defined for Android)
#  define OUZEL_PLATFORM_ANDROID 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENGLES 1
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#  define OUZEL_SUPPORTS_OPENSL 1
#  define OUZEL_SUPPORTS_OPENSLES 1

#elif defined(__linux__) // Linux
#  define OUZEL_PLATFORM_LINUX 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENAL 1
#  define OUZEL_SUPPORTS_ALSA 1

#  if defined(__x86_64__) || defined(__i386__) // x86 Linux
#    define OUZEL_OPENGL_INTERFACE_GLX 1
#    define OUZEL_SUPPORTS_X11 1
#  elif defined(__arm64__) || defined(__aarch64__) || defined(__arm__) // ARM Linux
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EGL 1
#  else
#    error "Unsupported architecture"
#  endif

#elif defined(__EMSCRIPTEN__) // Emscripten
#  define OUZEL_PLATFORM_EMSCRIPTEN 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENGLES 1
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#  define OUZEL_SUPPORTS_OPENAL 1

#else
# error "Unsupported platform"
#endif

#endif // OUZEL_PLATFORM_H
