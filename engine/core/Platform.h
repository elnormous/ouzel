#ifndef FLUXION_CORE_PLATFORM_H
#define FLUXION_CORE_PLATFORM_H

#if defined(_WIN32) // Windows
#  define FLUXION_PLATFORM_WINDOWS 1
#  define FLUXION_SUPPORTS_DIRECT3D 1
#  define FLUXION_SUPPORTS_DIRECT3D11 1
#  define FLUXION_SUPPORTS_OPENGL 1
#  define FLUXION_SUPPORTS_DIRECTSOUND 1
#  define FLUXION_SUPPORTS_WASAPI 1

#  if defined(_MSC_VER) // Visual Studio
#    define FLUXION_SUPPORTS_XAUDIO2 1
#  endif

#elif defined(__APPLE__) // macOS, iOS, tvOS
#  include <TargetConditionals.h>
#  define FLUXION_SUPPORTS_OPENAL 1
#  define FLUXION_SUPPORTS_COREAUDIO 1

#  if TARGET_OS_WATCH // watchOS
#    error "Apple Watch is not supported"
#  elif TARGET_OS_IOS // iOS
#    define FLUXION_PLATFORM_IOS 1
#    if !TARGET_OS_MACCATALYST
#      define FLUXION_SUPPORTS_OPENGL 1
#    endif
#    if !TARGET_OS_SIMULATOR
#      define FLUXION_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_TV // tvOS
#    define FLUXION_PLATFORM_TVOS 1
#    define FLUXION_SUPPORTS_OPENGL 1
#    if !TARGET_OS_SIMULATOR
#      define FLUXION_SUPPORTS_METAL 1
#    endif
#  elif TARGET_OS_MAC // any other Apple OS (check this last because it is defined for all Apple platforms)
#    define FLUXION_PLATFORM_MACOS 1
#    define FLUXION_SUPPORTS_OPENGL 1
#    if !TARGET_OS_SIMULATOR
#      define FLUXION_SUPPORTS_METAL 1
#    endif
#  endif

#elif defined(__ANDROID__) // Android (check this before Linux because __unix__ is also defined for Android)
#  define FLUXION_PLATFORM_ANDROID 1
#  define FLUXION_SUPPORTS_OPENGL 1
#  define FLUXION_SUPPORTS_OPENSL 1

#elif defined(__EMSCRIPTEN__) // Emscripten (check this before Linux because __unix__ is also defined for Emscripten)
#  define FLUXION_PLATFORM_EMSCRIPTEN 1
#  define FLUXION_SUPPORTS_OPENGL 1
#  define FLUXION_SUPPORTS_OPENAL 1

#elif defined(__unix__) // Linux/BSD/Solaris
#  define FLUXION_PLATFORM_LINUX 1
#  define FLUXION_SUPPORTS_OPENGL 1
#  define FLUXION_SUPPORTS_OPENAL 1
#  define FLUXION_SUPPORTS_ALSA 1

#  if defined(__x86_64__) || defined(__i386__) // x86 Linux
#    define FLUXION_SUPPORTS_X11 1
#  elif defined(__arm64__) || defined(__aarch64__) || defined(__arm__) // ARM Linux
#    define FLUXION_SUPPORTS_DISPMANX 1
#  else
#    error "Unsupported architecture"
#  endif

#else
#  error "Unsupported platform"
#endif

#endif // FLUXION_CORE_PLATFORM_H
