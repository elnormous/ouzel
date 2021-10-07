// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_SIMD_HPP
#define OUZEL_MATH_SIMD_HPP

#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
#  include <xmmintrin.h>
#  define OUZEL_SIMD_SSE
#endif
#if defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2
#  define OUZEL_SIMD_SSE2
#endif
#if defined(__ARM_NEON__)
#  include <arm_neon.h>
#  define OUZEL_SIMD_NEON
#endif

#endif // OUZEL_MATH_SIMD_HPP
