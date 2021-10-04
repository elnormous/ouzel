// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_SIMD_HPP
#define OUZEL_MATH_SIMD_HPP

#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP != 0
#  include <xmmintrin.h>
#  define OUZEL_SIMD_AVAILABLE
#  define OUZEL_SIMD_SSE
#elif defined(__ARM_NEON__)
#  include <arm_neon.h>
#  define OUZEL_SIMD_AVAILABLE
#  define OUZEL_SIMD_NEON
#endif

#endif // OUZEL_MATH_SIMD_HPP
