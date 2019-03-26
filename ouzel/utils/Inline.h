// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZE_UTILS_INLINE_H
#define OUZE_UTILS_INLINE_H

#if defined(_MSC_VER)
#  define ALWAYSINLINE __forceinline
#  define NOINLINE  __declspec(noinline)
#elif defined(__GNUC__)
#  define ALWAYSINLINE __attribute__((always_inline)) inline
#  define NOINLINE __attribute__((noinline))
#else
#  define ALWAYSINLINE inline
#  define NOINLINE
#endif

#endif // OUZE_UTILS_INLINE_H
