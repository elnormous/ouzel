// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined (__GLIBC__) || defined(__ANDROID__)
#  include <endian.h>
#  if (__BYTE_ORDER == __LITTLE_ENDIAN)
#    define OUZEL_LITTLE_ENDIAN
#  elif (__BYTE_ORDER == __BIG_ENDIAN)
#    define OUZEL_BIG_ENDIAN
#  else
#    error Unsupported endianness
#  endif

#elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN) || \
  defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) || \
  defined(__BIGENDIAN__) && !defined(__LITTLEENDIAN__) || \
  defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)
#  define OUZEL_BIG_ENDIAN
#elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN) || \
  defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) || \
  defined(__LITTLEENDIAN__) && !defined(__BIGENDIAN__) || \
  defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)
#  define OUZEL_LITTLE_ENDIAN
#elif defined(_POWER) || defined(__powerpc__) || \
  defined(__ppc__) || defined(_MIPSEB) || defined(_POWER) || \
  defined(__ARMEB__)
#  define OUZEL_BIG_ENDIAN
#elif defined(__i386__) || \
  defined(__ia64) || defined(__ia64__) || \
  defined(_M_IX86) || defined(_M_IA64) || \
  defined(__amd64) || \
  defined(__amd64__) || defined(_M_AMD64) || \
  defined(__x86_64) || defined(__x86_64__) || \
  defined(_M_X64) || \
  defined(__ARMEL__) || \
  (defined(_WIN32) && defined(__ARM__) && defined(_MSC_VER))
#  define OUZEL_LITTLE_ENDIAN
#else
#  error Unsupported endianness
#endif

#if defined(_WIN32) // Windows
#  define OUZEL_PLATFORM_WINDOWS 1
#  define OUZEL_SUPPORTS_DIRECT3D 1
#  define OUZEL_SUPPORTS_DIRECT3D11 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_OPENGL_INTERFACE_WGL 1
#  define OUZEL_SUPPORTS_DIRECTSOUND 1

#  if defined(_MSC_VER) // Visual Studio
#    define OUZEL_SUPPORTS_XAUDIO2 1
#  endif

#  define OUZEL_MULTITHREADED 1
#  define OUZEL_ARCHITECTURE_X86

#  if defined(_M_X64)
#    define OUZEL_64BITS 1
#  elif defined(_M_IX86)
#    define OUZEL_32BITS 1
#  else
#    error "Unsupported architecture"
#  endif

#elif defined(__APPLE__) // macOS, iOS, tvOS
#  include <TargetConditionals.h>
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENAL 1
#  define OUZEL_SUPPORTS_COREAUDIO 1

#  if TARGET_OS_IOS
#    define OUZEL_PLATFORM_IOS 1
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EAGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_TV
#    define OUZEL_PLATFORM_TVOS 1
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EAGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_MAC
#    define OUZEL_PLATFORM_MACOS 1
#    define OUZEL_OPENGL_INTERFACE_CGL 1
#    if !TARGET_OS_SIMULATOR
#      define OUZEL_SUPPORTS_METAL 1
#    endif
#  endif

#  define OUZEL_MULTITHREADED 1

#  if defined(__x86_64__) || defined(__i386__)
#    define OUZEL_ARCHITECTURE_X86
#    if defined(__x86_64__)
#      define OUZEL_64BITS 1
#    elif defined(__i386__)
#      define OUZEL_32BITS 1
#    endif
#  elif defined(__arm64__) || defined(__arm__)
#    define OUZEL_ARCHITECTURE_ARM
#    if defined(__arm64__)
#      define OUZEL_64BITS 1
#    elif defined(__arm__)
#      define OUZEL_32BITS 1
#    endif
#    if defined(__ARM_NEON__)
#      define OUZEL_SUPPORTS_NEON 1
#    endif
#  else
#    error "Unsupported architecture"
#  endif

#elif defined(__ANDROID__) // Android
#  define OUZEL_PLATFORM_ANDROID 1
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENGLES 1
#  define OUZEL_OPENGL_INTERFACE_EGL 1
#  define OUZEL_SUPPORTS_OPENSL 1
#  define OUZEL_SUPPORTS_OPENSLES 1
#  define OUZEL_MULTITHREADED 1

#  if defined(__x86_64__) || defined(__i386__)
#    define OUZEL_ARCHITECTURE_X86
#    if defined(__x86_64__)
#      define OUZEL_64BITS 1
#    elif defined(__i386__)
#      define OUZEL_32BITS 1
#    endif
#  elif defined(__arm64__) || defined(__aarch64__) || defined(__arm__)
#    define OUZEL_ARCHITECTURE_ARM
#    if defined(__arm64__) || defined(__aarch64__)
#      define OUZEL_64BITS 1
#    elif defined(__arm__)
#      define OUZEL_32BITS 1
#    endif
#    if defined(__ARM_NEON__)
#      define OUZEL_SUPPORTS_NEON 1
#      if defined(__arm__) // NEON support must be checked on Android for 32-bit platforms
#        define OUZEL_SUPPORTS_NEON_CHECK 1
#      endif
#    endif
#  else
#    error "Unsupported architecture"
#  endif

#elif defined(__linux__) // Linux
#  define OUZEL_SUPPORTS_OPENGL 1
#  define OUZEL_SUPPORTS_OPENAL 1
#  define OUZEL_SUPPORTS_ALSA 1
#  define OUZEL_MULTITHREADED 1

#  if defined(__x86_64__) || defined(__i386__) // x86 Linux
#    define OUZEL_PLATFORM_LINUX 1
#    define OUZEL_OPENGL_INTERFACE_GLX 1
#    define OUZEL_SUPPORTS_X11 1
#    define OUZEL_ARCHITECTURE_X86

#    if defined(__x86_64__)
#      define OUZEL_64BITS 1
#    elif defined(__i386__)
#      define OUZEL_32BITS 1
#    endif
#  elif defined(__arm64__) || defined(__aarch64__) || defined(__arm__) // ARM Linux
#    define OUZEL_PLATFORM_RASPBIAN 1 // TODO: merge with x86 linux
#    define OUZEL_SUPPORTS_OPENGLES 1
#    define OUZEL_OPENGL_INTERFACE_EGL 1
#    define OUZEL_ARCHITECTURE_ARM

#    if defined(__arm64__) || defined(__aarch64__)
#      define OUZEL_64BITS 1
#    elif defined(__arm__)
#      define OUZEL_32BITS 1
#    endif

#    if defined(__ARM_NEON__)
#      define OUZEL_SUPPORTS_NEON 1
#    endif
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

#if defined(__SSE__)
#  define OUZEL_SUPPORTS_SSE 1
#endif
