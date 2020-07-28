// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGL_H
#define OUZEL_GRAPHICS_OGL_H

#if defined(_WIN32) // Windows
#  define OUZEL_OPENGL_INTERFACE_WGL 1
#elif defined(__APPLE__) // macOS, iOS, tvOS
#  include <TargetConditionals.h>
#  if TARGET_OS_WATCH // watchOS
#    error "Apple Watch is not supported"
#  elif TARGET_OS_IOS || TARGET_OS_TV // iOS, tvOS
#    define OUZEL_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EAGL 1
#  elif TARGET_OS_MAC // any other Apple OS (check this last because it is defined for all Apple platforms)
#    define OUZEL_OPENGL_INTERFACE_CGL 1
#  endif
#elif defined(__ANDROID__) // Android (check this before Linux because __linux__ is also defined for Android)
#  define OUZEL_OPENGLES 1
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#elif defined(__linux__) // Linux
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#  if defined(__x86_64__) || defined(__i386__) // x86 Linux
#  elif defined(__arm64__) || defined(__aarch64__) || defined(__arm__) // ARM Linux
#    define OUZEL_OPENGLES 1
#  else
#    error "Unsupported architecture"
#  endif
#elif defined(__EMSCRIPTEN__) // Emscripten
#  define OUZEL_OPENGLES 1
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#else
#  error "Unsupported platform"
#endif

#endif // OUZEL_GRAPHICS_OGL_H
