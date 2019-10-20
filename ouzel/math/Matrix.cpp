// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#if defined(__SSE__)
#  include <xmmintrin.h>
#endif
#include "Matrix.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    template <>
    void Matrix<4, 4, float>::add(float scalar, Matrix& dst)
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]\n\t" // m[0-7] m[8-15]
                "ld1r {v4.4s}, [%2]\n\t" // ssss

                "fadd v8.4s, v0.4s, v4.4s\n\t" // dst.m[0-3] = m[0-3] + s
                "fadd v9.4s, v1.4s, v4.4s\n\t" // dst.m[4-7] = m[4-7] + s
                "fadd v10.4s, v2.4s, v4.4s\n\t" // dst.m[8-11] = m[8-11] + s
                "fadd v11.4s, v3.4s, v4.4s\n\t" // dst.m[12-15] = m[12-15] + s

                "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0]\n\t" // result in V9
                : // output
                : "r"(dst.m), "r"(m), "r"(&scalar) // input
                : "v0", "v1", "v2", "v3", "v4", "v8", "v9", "v10", "v11", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {q0, q1}, [%1]!\n\t" // m[0-7]
                "vld1.32 {q2, q3}, [%1]\n\t" // m[8-15]
                "vld1.32 {d8[0]}, [%2]\n\t" // s
                "vmov.f32 s17, s16\n\t" // s
                "vmov.f32 s18, s16\n\t" // s
                "vmov.f32 s19, s16\n\t" // s

                "vadd.f32 q8, q0, q4\n\t" // dst.m[0-3] = m[0-3] + s
                "vadd.f32 q9, q1, q4\n\t" // dst.m[4-7] = m[4-7] + s
                "vadd.f32 q10, q2, q4\n\t" // dst.m[8-11] = m[8-11] + s
                "vadd.f32 q11, q3, q4\n\t" // dst.m[12-15] = m[12-15] + s

                "vst1.32 {q8, q9}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q10, q11}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m), "r"(&scalar) // input
                : "q0", "q1", "q2", "q3", "q4", "q8", "q9", "q10", "q11", "memory"
            );
#  endif
#elif defined(__SSE__)
            __m128 s = _mm_set1_ps(scalar);
            _mm_store_ps(&dst.m[0], _mm_add_ps(_mm_load_ps(&m[0]), s));
            _mm_store_ps(&dst.m[4], _mm_add_ps(_mm_load_ps(&m[4]), s));
            _mm_store_ps(&dst.m[8], _mm_add_ps(_mm_load_ps(&m[8]), s));
            _mm_store_ps(&dst.m[12], _mm_add_ps(_mm_load_ps(&m[12]), s));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            dst.m[0] = m[0] + scalar;
            dst.m[1] = m[1] + scalar;
            dst.m[2] = m[2] + scalar;
            dst.m[3] = m[3] + scalar;
            dst.m[4] = m[4] + scalar;
            dst.m[5] = m[5] + scalar;
            dst.m[6] = m[6] + scalar;
            dst.m[7] = m[7] + scalar;
            dst.m[8] = m[8] + scalar;
            dst.m[9] = m[9] + scalar;
            dst.m[10] = m[10] + scalar;
            dst.m[11] = m[11] + scalar;
            dst.m[12] = m[12] + scalar;
            dst.m[13] = m[13] + scalar;
            dst.m[14] = m[14] + scalar;
            dst.m[15] = m[15] + scalar;
        }
    }

    template <>
    void Matrix<4, 4, float>::add(const Matrix& m1, const Matrix& m2, Matrix& dst)
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]\n\t" // m1.m[0-7] m1.m[8-15]
                "ld4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%2]\n\t" // m2.m[0-7] m2.m[8-15]

                "fadd v12.4s, v0.4s, v8.4s\n\t" // dst.m[0-3] = m1.m[0-3] + m2.m[0-3]
                "fadd v13.4s, v1.4s, v9.4s\n\t" // dst.m[4-7] = m1.m[4-7] + m2.m[4-7]
                "fadd v14.4s, v2.4s, v10.4s\n\t" // dst.m[8-11] = m1.m[8-11] + m2.m[8-11]
                "fadd v15.4s, v3.4s, v11.4s\n\t" // dst.m[12-15] = m1.m[12-15] + m2.m[12-15]

                "st4 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]\n\t" // dst.m[0-7] dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {q0, q1}, [%1]!\n\t" // m1.m[0-7]
                "vld1.32 {q2, q3}, [%1]\n\t" // m1.m[8-15]
                "vld1.32 {q8, q9}, [%2]!\n\t" // m2.m[0-7]
                "vld1.32 {q10, q11}, [%2]\n\t" // m2.m[8-15]

                "vadd.f32 q12, q0, q8\n\t" // dst.m[0-3] = m1.m[0-3] + m2.m[0-3]
                "vadd.f32 q13, q1, q9\n\t" // dst.m[4-7] = m1.m[4-7] + m2.m[4-7]
                "vadd.f32 q14, q2, q10\n\t" // dst.m[8-11] = m1.m[8-11] + m2.m[8-11]
                "vadd.f32 q15, q3, q11\n\t" // dst.m[12-15] = m1.m[12-15] + m2.m[12-15]

                "vst1.32 {q12, q13}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q14, q15}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory"
            );
#  endif
#elif defined(__SSE__)
            _mm_store_ps(&dst.m[0], _mm_add_ps(_mm_load_ps(&m1.m[0]), _mm_load_ps(&m2.m[0])));
            _mm_store_ps(&dst.m[4], _mm_add_ps(_mm_load_ps(&m1.m[4]), _mm_load_ps(&m2.m[4])));
            _mm_store_ps(&dst.m[8], _mm_add_ps(_mm_load_ps(&m1.m[8]), _mm_load_ps(&m2.m[8])));
            _mm_store_ps(&dst.m[12], _mm_add_ps(_mm_load_ps(&m1.m[12]), _mm_load_ps(&m2.m[12])));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            dst.m[0] = m1.m[0] + m2.m[0];
            dst.m[1] = m1.m[1] + m2.m[1];
            dst.m[2] = m1.m[2] + m2.m[2];
            dst.m[3] = m1.m[3] + m2.m[3];
            dst.m[4] = m1.m[4] + m2.m[4];
            dst.m[5] = m1.m[5] + m2.m[5];
            dst.m[6] = m1.m[6] + m2.m[6];
            dst.m[7] = m1.m[7] + m2.m[7];
            dst.m[8] = m1.m[8] + m2.m[8];
            dst.m[9] = m1.m[9] + m2.m[9];
            dst.m[10] = m1.m[10] + m2.m[10];
            dst.m[11] = m1.m[11] + m2.m[11];
            dst.m[12] = m1.m[12] + m2.m[12];
            dst.m[13] = m1.m[13] + m2.m[13];
            dst.m[14] = m1.m[14] + m2.m[14];
            dst.m[15] = m1.m[15] + m2.m[15];
        }
    }

    template <size_t C, size_t R, typename T>
    void Matrix<C, R, T>::invert(Matrix& dst) const
    {
        const T a0 = m[0] * m[5] - m[1] * m[4];
        const T a1 = m[0] * m[6] - m[2] * m[4];
        const T a2 = m[0] * m[7] - m[3] * m[4];
        const T a3 = m[1] * m[6] - m[2] * m[5];
        const T a4 = m[1] * m[7] - m[3] * m[5];
        const T a5 = m[2] * m[7] - m[3] * m[6];
        const T b0 = m[8] * m[13] - m[9] * m[12];
        const T b1 = m[8] * m[14] - m[10] * m[12];
        const T b2 = m[8] * m[15] - m[11] * m[12];
        const T b3 = m[9] * m[14] - m[10] * m[13];
        const T b4 = m[9] * m[15] - m[11] * m[13];
        const T b5 = m[10] * m[15] - m[11] * m[14];

        // Calculate the determinant
        const T det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

        // Close to zero, can't invert
        if (std::fabs(det) <= std::numeric_limits<T>::min()) return;

        Matrix inverse;
        inverse.m[0] = m[5] * b5 - m[6] * b4 + m[7] * b3;
        inverse.m[1] = -m[1] * b5 + m[2] * b4 - m[3] * b3;
        inverse.m[2] = m[13] * a5 - m[14] * a4 + m[15] * a3;
        inverse.m[3] = -m[9] * a5 + m[10] * a4 - m[11] * a3;

        inverse.m[4] = -m[4] * b5 + m[6] * b2 - m[7] * b1;
        inverse.m[5] = m[0] * b5 - m[2] * b2 + m[3] * b1;
        inverse.m[6] = -m[12] * a5 + m[14] * a2 - m[15] * a1;
        inverse.m[7] = m[8] * a5 - m[10] * a2 + m[11] * a1;

        inverse.m[8] = m[4] * b4 - m[5] * b2 + m[7] * b0;
        inverse.m[9] = -m[0] * b4 + m[1] * b2 - m[3] * b0;
        inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
        inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

        inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
        inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
        inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
        inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

        multiply(inverse, 1 / det, dst);
    }

    template <>
    void Matrix<4, 4, float>::multiply(const Matrix& m, float scalar, Matrix& dst)
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld1 {v0.s}[0], [%2]\n\t" // s
                "ld4 {v4.4s, v5.4s, v6.4s, v7.4s}, [%1]\n\t" // m.m[0-7] m.m[8-15]

                "fmul v8.4s, v4.4s, v0.s[0]\n\t" // dst.m[0-3] = m.m[0-3] * s
                "fmul v9.4s, v5.4s, v0.s[0]\n\t" // dst.m[4-7] = m.m[4-7] * s
                "fmul v10.4s, v6.4s, v0.s[0]\n\t" // dst.m[8-11] = m.m[8-11] * s
                "fmul v11.4s, v7.4s, v0.s[0]\n\t" // dst.m[12-15] = m.m[12-15] * s

                "st4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%0]\n\t" // dst.m[0-7] dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m.m), "r"(&scalar) // input
                : "v0", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {d0[0]}, [%2]\n\t" // m.m[0-7]
                "vld1.32 {q4-q5}, [%1]!\n\t" // m.m[8-15]
                "vld1.32 {q6-q7}, [%1]\n\t" // s

                "vmul.f32 q8, q4, d0[0]\n\t" // dst.m[0-3] = m.m[0-3] * s
                "vmul.f32 q9, q5, d0[0]\n\t" // dst.m[4-7] = m.m[4-7] * s
                "vmul.f32 q10, q6, d0[0]\n\t" // dst.m[8-11] = m.m[8-11] * s
                "vmul.f32 q11, q7, d0[0]\n\t" // dst.m[12-15] = m.m[12-15] * s

                "vst1.32 {q8-q9}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q10-q11}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m.m), "r"(&scalar) // input
                : "q0", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "memory"
            );
#  endif
#elif defined(__SSE__)
            __m128 s = _mm_set1_ps(scalar);
            _mm_store_ps(&dst.m[0], _mm_mul_ps(_mm_load_ps(&m.m[0]), s));
            _mm_store_ps(&dst.m[4], _mm_mul_ps(_mm_load_ps(&m.m[4]), s));
            _mm_store_ps(&dst.m[8], _mm_mul_ps(_mm_load_ps(&m.m[8]), s));
            _mm_store_ps(&dst.m[12], _mm_mul_ps(_mm_load_ps(&m.m[12]), s));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            dst.m[0] = m.m[0] * scalar;
            dst.m[1] = m.m[1] * scalar;
            dst.m[2] = m.m[2] * scalar;
            dst.m[3] = m.m[3] * scalar;
            dst.m[4] = m.m[4] * scalar;
            dst.m[5] = m.m[5] * scalar;
            dst.m[6] = m.m[6] * scalar;
            dst.m[7] = m.m[7] * scalar;
            dst.m[8] = m.m[8] * scalar;
            dst.m[9] = m.m[9] * scalar;
            dst.m[10] = m.m[10] * scalar;
            dst.m[11] = m.m[11] * scalar;
            dst.m[12] = m.m[12] * scalar;
            dst.m[13] = m.m[13] * scalar;
            dst.m[14] = m.m[14] * scalar;
            dst.m[15] = m.m[15] * scalar;
        }
    }

    template <>
    void Matrix<4, 4, float>::multiply(const Matrix& m1, const Matrix& m2, Matrix& dst)
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld1 {v8.4s, v9.4s, v10.4s, v11.4s}, [%1]\n\t" // m1.m[0-7] m1.m[8-15]
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%2]\n\t" // m2.m[0-15]

                "fmul v12.4s, v8.4s, v0.s[0]\n\t" // dst.m[0-3] = m1.m[0-3] * m2.m[0]
                "fmul v13.4s, v8.4s, v0.s[1]\n\t" // dst.m[4-7] = m1.m[4-7] * m2.m[4]
                "fmul v14.4s, v8.4s, v0.s[2]\n\t" // dst.m[8-11] = m1.m[8-11] * m2.m[8]
                "fmul v15.4s, v8.4s, v0.s[3]\n\t" // dst.m[12-15] = m1.m[12-15] * m2.m[12]

                "fmla v12.4s, v9.4s, v1.s[0]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[1]
                "fmla v13.4s, v9.4s, v1.s[1]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[5]
                "fmla v14.4s, v9.4s, v1.s[2]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[9]
                "fmla v15.4s, v9.4s, v1.s[3]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[13]

                "fmla v12.4s, v10.4s, v2.s[0]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[2]
                "fmla v13.4s, v10.4s, v2.s[1]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[6]
                "fmla v14.4s, v10.4s, v2.s[2]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[10]
                "fmla v15.4s, v10.4s, v2.s[3]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[14]

                "fmla v12.4s, v11.4s, v3.s[0]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[3]
                "fmla v13.4s, v11.4s, v3.s[1]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[7]
                "fmla v14.4s, v11.4s, v3.s[2]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[11]
                "fmla v15.4s, v11.4s, v3.s[3]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[15]

                "st1 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]\n\t" // dst.m[0-7] dst.m[8-15]

                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "memory", "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {d16 - d19}, [%1]!\n\t" // m1.m[0-7]
                "vld1.32 {d20 - d23}, [%1]\n\t" // m1.m[8-15]
                "vld1.32 {d0 - d3}, [%2]!\n\t" // m2.m[0-7]
                "vld1.32 {d4 - d7}, [%2]\n\t" // m2.m[8-15]

                "vmul.f32 q12, q8, d0[0]\n\t" // dst.m[0-3] = m1.m[0-3] * m2.m[0]
                "vmul.f32 q13, q8, d2[0]\n\t" // dst.m[4-7] = m1.m[4-7] * m2.m[4]
                "vmul.f32 q14, q8, d4[0]\n\t" // dst.m[8-11] = m1.m[8-11] * m2.m[8]
                "vmul.f32 q15, q8, d6[0]\n\t" // dst.m[12-15] = m1.m[12-15] * m2.m[12]

                "vmla.f32 q12, q9, d0[1]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[1]
                "vmla.f32 q13, q9, d2[1]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[5]
                "vmla.f32 q14, q9, d4[1]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[9]
                "vmla.f32 q15, q9, d6[1]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[13]

                "vmla.f32 q12, q10, d1[0]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[2]
                "vmla.f32 q13, q10, d3[0]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[6]
                "vmla.f32 q14, q10, d5[0]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[10]
                "vmla.f32 q15, q10, d7[0]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[14]

                "vmla.f32 q12, q11, d1[1]\n\t" // dst.m[0-3] += m1.m[0-3] * m2.m[3]
                "vmla.f32 q13, q11, d3[1]\n\t" // dst.m[4-7] += m1.m[4-7] * m2.m[7]
                "vmla.f32 q14, q11, d5[1]\n\t" // dst.m[8-11] += m1.m[8-11] * m2.m[11]
                "vmla.f32 q15, q11, d7[1]\n\t" // dst.m[12-15] += m1.m[12-15] * m2.m[15]

                "vst1.32 {d24 - d27}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {d28 - d31}, [%0]\n\t" // dst.m[8-15]

                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
#  endif
#elif defined(__SSE__)
            // keep the result in a separate array in case m1 is the same as m2
            __m128 dest[4];

            {
                __m128 e0 = _mm_set1_ps(m2.m[0]);
                __m128 e1 = _mm_set1_ps(m2.m[1]);
                __m128 e2 = _mm_set1_ps(m2.m[2]);
                __m128 e3 = _mm_set1_ps(m2.m[3]);

                __m128 v0 = _mm_mul_ps(_mm_load_ps(&m1.m[0]), e0);
                __m128 v1 = _mm_mul_ps(_mm_load_ps(&m1.m[4]), e1);
                __m128 v2 = _mm_mul_ps(_mm_load_ps(&m1.m[8]), e2);
                __m128 v3 = _mm_mul_ps(_mm_load_ps(&m1.m[12]), e3);

                __m128 a0 = _mm_add_ps(v0, v1);
                __m128 a1 = _mm_add_ps(v2, v3);
                __m128 a2 = _mm_add_ps(a0, a1);

                dest[0] = a2;
            }

            {
                __m128 e0 = _mm_set1_ps(m2.m[4]);
                __m128 e1 = _mm_set1_ps(m2.m[5]);
                __m128 e2 = _mm_set1_ps(m2.m[6]);
                __m128 e3 = _mm_set1_ps(m2.m[7]);

                __m128 v0 = _mm_mul_ps(_mm_load_ps(&m1.m[0]), e0);
                __m128 v1 = _mm_mul_ps(_mm_load_ps(&m1.m[4]), e1);
                __m128 v2 = _mm_mul_ps(_mm_load_ps(&m1.m[8]), e2);
                __m128 v3 = _mm_mul_ps(_mm_load_ps(&m1.m[12]), e3);

                __m128 a0 = _mm_add_ps(v0, v1);
                __m128 a1 = _mm_add_ps(v2, v3);
                __m128 a2 = _mm_add_ps(a0, a1);

                dest[1] = a2;
            }

            {
                __m128 e0 = _mm_set1_ps(m2.m[8]);
                __m128 e1 = _mm_set1_ps(m2.m[9]);
                __m128 e2 = _mm_set1_ps(m2.m[10]);
                __m128 e3 = _mm_set1_ps(m2.m[11]);

                __m128 v0 = _mm_mul_ps(_mm_load_ps(&m1.m[0]), e0);
                __m128 v1 = _mm_mul_ps(_mm_load_ps(&m1.m[4]), e1);
                __m128 v2 = _mm_mul_ps(_mm_load_ps(&m1.m[8]), e2);
                __m128 v3 = _mm_mul_ps(_mm_load_ps(&m1.m[12]), e3);

                __m128 a0 = _mm_add_ps(v0, v1);
                __m128 a1 = _mm_add_ps(v2, v3);
                __m128 a2 = _mm_add_ps(a0, a1);

                dest[2] = a2;
            }

            {
                __m128 e0 = _mm_set1_ps(m2.m[12]);
                __m128 e1 = _mm_set1_ps(m2.m[13]);
                __m128 e2 = _mm_set1_ps(m2.m[14]);
                __m128 e3 = _mm_set1_ps(m2.m[15]);

                __m128 v0 = _mm_mul_ps(_mm_load_ps(&m1.m[0]), e0);
                __m128 v1 = _mm_mul_ps(_mm_load_ps(&m1.m[4]), e1);
                __m128 v2 = _mm_mul_ps(_mm_load_ps(&m1.m[8]), e2);
                __m128 v3 = _mm_mul_ps(_mm_load_ps(&m1.m[12]), e3);

                __m128 a0 = _mm_add_ps(v0, v1);
                __m128 a1 = _mm_add_ps(v2, v3);
                __m128 a2 = _mm_add_ps(a0, a1);

                dest[3] = a2;
            }

            _mm_store_ps(&dst.m[0], dest[0]);
            _mm_store_ps(&dst.m[4], dest[1]);
            _mm_store_ps(&dst.m[8], dest[2]);
            _mm_store_ps(&dst.m[12], dest[3]);
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            const float product[16] {
                m1.m[0] * m2.m[0] + m1.m[4] * m2.m[1] + m1.m[8] * m2.m[2] + m1.m[12] * m2.m[3],
                m1.m[1] * m2.m[0] + m1.m[5] * m2.m[1] + m1.m[9] * m2.m[2] + m1.m[13] * m2.m[3],
                m1.m[2] * m2.m[0] + m1.m[6] * m2.m[1] + m1.m[10] * m2.m[2] + m1.m[14] * m2.m[3],
                m1.m[3] * m2.m[0] + m1.m[7] * m2.m[1] + m1.m[11] * m2.m[2] + m1.m[15] * m2.m[3],

                m1.m[0] * m2.m[4] + m1.m[4] * m2.m[5] + m1.m[8] * m2.m[6] + m1.m[12] * m2.m[7],
                m1.m[1] * m2.m[4] + m1.m[5] * m2.m[5] + m1.m[9] * m2.m[6] + m1.m[13] * m2.m[7],
                m1.m[2] * m2.m[4] + m1.m[6] * m2.m[5] + m1.m[10] * m2.m[6] + m1.m[14] * m2.m[7],
                m1.m[3] * m2.m[4] + m1.m[7] * m2.m[5] + m1.m[11] * m2.m[6] + m1.m[15] * m2.m[7],

                m1.m[0] * m2.m[8] + m1.m[4] * m2.m[9] + m1.m[8] * m2.m[10] + m1.m[12] * m2.m[11],
                m1.m[1] * m2.m[8] + m1.m[5] * m2.m[9] + m1.m[9] * m2.m[10] + m1.m[13] * m2.m[11],
                m1.m[2] * m2.m[8] + m1.m[6] * m2.m[9] + m1.m[10] * m2.m[10] + m1.m[14] * m2.m[11],
                m1.m[3] * m2.m[8] + m1.m[7] * m2.m[9] + m1.m[11] * m2.m[10] + m1.m[15] * m2.m[11],

                m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8] * m2.m[14] + m1.m[12] * m2.m[15],
                m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9] * m2.m[14] + m1.m[13] * m2.m[15],
                m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15],
                m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15]
            };

            std::copy(std::begin(product), std::end(product), dst.m);
        }
    }

    template <>
    void Matrix<4, 4, float>::negate(Matrix& dst) const
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]\n\t" // m[0-7] m[8-15]

                "fneg v4.4s, v0.4s\n\t" // -m[0-3]
                "fneg v5.4s, v1.4s\n\t" // -m[4-7]
                "fneg v6.4s, v2.4s\n\t" // -m[8-11]
                "fneg v7.4s, v3.4s\n\t" // -m[12-15]

                "st4 {v4.4s, v5.4s, v6.4s, v7.4s}, [%0]\n\t" // dst.m[0-7] dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m) // input
                : "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {q0-q1}, [%1]!\n\t" // m[0-7]
                "vld1.32 {q2-q3}, [%1]\n\t" // m[8-15]

                "vneg.f32 q4, q0\n\t" // -m[0-3]
                "vneg.f32 q5, q1\n\t" // -m[4-7]
                "vneg.f32 q6, q2\n\t" // -m[8-11]
                "vneg.f32 q7, q3\n\t" // -m[12-15]

                "vst1.32 {q4-q5}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q6-q7}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m) // input
                : "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "memory"
            );
#  endif
#elif defined(__SSE__)
            __m128 z = _mm_setzero_ps();
            _mm_store_ps(&dst.m[0], _mm_sub_ps(z, _mm_load_ps(&m[0])));
            _mm_store_ps(&dst.m[4], _mm_sub_ps(z, _mm_load_ps(&m[4])));
            _mm_store_ps(&dst.m[8], _mm_sub_ps(z, _mm_load_ps(&m[8])));
            _mm_store_ps(&dst.m[12], _mm_sub_ps(z, _mm_load_ps(&m[12])));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            dst.m[0] = -m[0];
            dst.m[1] = -m[1];
            dst.m[2] = -m[2];
            dst.m[3] = -m[3];
            dst.m[4] = -m[4];
            dst.m[5] = -m[5];
            dst.m[6] = -m[6];
            dst.m[7] = -m[7];
            dst.m[8] = -m[8];
            dst.m[9] = -m[9];
            dst.m[10] = -m[10];
            dst.m[11] = -m[11];
            dst.m[12] = -m[12];
            dst.m[13] = -m[13];
            dst.m[14] = -m[14];
            dst.m[15] = -m[15];
        }
    }

    template <>
    void Matrix<4, 4, float>::subtract(const Matrix& m1, const Matrix& m2, Matrix& dst)
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]\n\t" // m1.m[0-7] m1.m[8-15]
                "ld4 {v8.4s, v9.4s, v10.4s, v11.4s}, [%2]\n\t" // m2.m[0-7] m2.m[8-15]

                "fsub v12.4s, v0.4s, v8.4s\n\t" // dst.m[0-3] = m1.m[0-3] - m2.m[0-3]
                "fsub v13.4s, v1.4s, v9.4s\n\t" // dst.m[4-7] = m1.m[4-7] - m2.m[4-7]
                "fsub v14.4s, v2.4s, v10.4s\n\t" // dst.m[8-11] = m1.m[8-11] - m2.m[8-11]
                "fsub v15.4s, v3.4s, v11.4s\n\t" // dst.m[12-15] = m1.m[12-15] - m2.m[12-15]

                "st4 {v12.4s, v13.4s, v14.4s, v15.4s}, [%0]\n\t" // dst.m[0-7] dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "v0", "v1", "v2", "v3", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {q0, q1}, [%1]!\n\t" // m1.m[0-7]
                "vld1.32 {q2, q3}, [%1]\n\t" // m1.m[8-15]
                "vld1.32 {q8, q9}, [%2]!\n\t" // m2.m[0-7]
                "vld1.32 {q10, q11}, [%2]\n\t" // m2.m[8-15]

                "vsub.f32 q12, q0, q8\n\t" // dst.m[0-3] = m1.m[0-3] - m2.m[0-3]
                "vsub.f32 q13, q1, q9\n\t" // dst.m[4-7] = m1.m[4-7] - m2.m[4-7]
                "vsub.f32 q14, q2, q10\n\t" // dst.m[8-11] = m1.m[8-11] - m2.m[8-11]
                "vsub.f32 q15, q3, q11\n\t" // dst.m[12-15] = m1.m[12-15] - m2.m[12-15]

                "vst1.32 {q12, q13}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q14, q15}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m1.m), "r"(m2.m) // input
                : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory"
            );
#  endif
#elif defined(__SSE__)
            _mm_store_ps(&dst.m[0], _mm_sub_ps(_mm_load_ps(&m1.m[0]), _mm_load_ps(&m2.m[0])));
            _mm_store_ps(&dst.m[4], _mm_sub_ps(_mm_load_ps(&m1.m[4]), _mm_load_ps(&m2.m[4])));
            _mm_store_ps(&dst.m[8], _mm_sub_ps(_mm_load_ps(&m1.m[8]), _mm_load_ps(&m2.m[8])));
            _mm_store_ps(&dst.m[12], _mm_sub_ps(_mm_load_ps(&m1.m[12]), _mm_load_ps(&m2.m[12])));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            dst.m[0] = m1.m[0] - m2.m[0];
            dst.m[1] = m1.m[1] - m2.m[1];
            dst.m[2] = m1.m[2] - m2.m[2];
            dst.m[3] = m1.m[3] - m2.m[3];
            dst.m[4] = m1.m[4] - m2.m[4];
            dst.m[5] = m1.m[5] - m2.m[5];
            dst.m[6] = m1.m[6] - m2.m[6];
            dst.m[7] = m1.m[7] - m2.m[7];
            dst.m[8] = m1.m[8] - m2.m[8];
            dst.m[9] = m1.m[9] - m2.m[9];
            dst.m[10] = m1.m[10] - m2.m[10];
            dst.m[11] = m1.m[11] - m2.m[11];
            dst.m[12] = m1.m[12] - m2.m[12];
            dst.m[13] = m1.m[13] - m2.m[13];
            dst.m[14] = m1.m[14] - m2.m[14];
            dst.m[15] = m1.m[15] - m2.m[15];
        }
    }

    template <>
    void Matrix<4, 4, float>::transformVector(const Vector<4, float>& v, Vector<4, float>& dst) const
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld1 {v0.4s}, [%1]\n\t" // v[0, 1, 2, 3]
                "ld1 {v9.4s, v10.4s, v11.4s, v12.4s}, [%2]\n\t" // m[0-7] m[8-15]

                "fmul v13.4s, v9.4s, v0.s[0]\n\t" // dst.v = m[0-3] * v.v[0]
                "fmla v13.4s, v10.4s, v0.s[1]\n\t" // dst.v = m[4-7] * v.v[1]
                "fmla v13.4s, v11.4s, v0.s[2]\n\t" // dst.v = m[8-11] * v.v[2]
                "fmla v13.4s, v12.4s, v0.s[3]\n\t" // dst.v = m[12-15] * v.v[3]

                "st1 {v13.4s}, [%0]\n\t" // dst.v[0, 1, 2, 3]
                : // output
                : "r"(dst.v), "r"(v.v), "r"(m) // input
                : "v0", "v9", "v10","v11", "v12", "v13", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld1.32 {d0, d1}, [%1]\n\t" // v.v[0, 1, 2, 3]
                "vld1.32 {d18 - d21}, [%2]!\n\t" // m[0-7]
                "vld1.32 {d22 - d25}, [%2]\n\t" // m[8-15]

                "vmul.f32 q13, q9, d0[0]\n\t" // dst.v = m[0-3] * v.v[0]
                "vmla.f32 q13, q10, d0[1]\n\t" // dst.v = m[4-7] * v.v[1]
                "vmla.f32 q13, q11, d1[0]\n\t" // dst.v = m[8-11] * v.v[2]
                "vmla.f32 q13, q12, d1[1]\n\t" // dst.v = m[12-15] * v.v[3]

                "vst1.32 {d26, d27}, [%0]\n\t" // dst.v[0, 1, 2, 3]
                : // output
                : "r"(dst.v), "r"(v.v), "r"(m) // input
                : "q0", "q9", "q10","q11", "q12", "q13", "memory"
             );
#  endif
#elif defined(__SSE__)
            __m128 col1 = _mm_set1_ps(v.v[0]);
            __m128 col2 = _mm_set1_ps(v.v[1]);
            __m128 col3 = _mm_set1_ps(v.v[2]);
            __m128 col4 = _mm_set1_ps(v.v[3]);

            __m128 s = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_load_ps(&m[0]), col1),
                                             _mm_mul_ps(_mm_load_ps(&m[4]), col2)),
                                  _mm_add_ps(_mm_mul_ps(_mm_load_ps(&m[8]), col3),
                                             _mm_mul_ps(_mm_load_ps(&m[12]), col4)));
            _mm_store_ps(dst.v, s);
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            assert(&v != &dst);
            dst.v[0] = v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8] + v.v[3] * m[12];
            dst.v[1] = v.v[0] * m[1] + v.v[1] * m[5] + v.v[2] * m[9] + v.v[3] * m[13];
            dst.v[2] = v.v[0] * m[2] + v.v[1] * m[6] + v.v[2] * m[10] + v.v[3] * m[14];
            dst.v[3] = v.v[0] * m[3] + v.v[1] * m[7] + v.v[2] * m[11] + v.v[3] * m[15];
        }
    }

    template <size_t C, size_t R, typename T>
    void Matrix<C, R, T>::transpose(Matrix& dst) const
    {
        const T t[16] = {
            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]
        };
        std::copy(std::begin(t), std::end(t), dst.m);
    }

    template <>
    void Matrix<4, 4, float>::transpose(Matrix& dst) const
    {
        if (isSimdAvailable)
        {
#if defined(__ARM_NEON__)
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
            asm volatile
            (
                "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [%1]\n\t" // dst.m[0, 4, 8, 12] = m[0-3]
                "st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [%0]\n\t" // dst.m[1, 5, 9, 13] = m[4-7]
                : // output
                : "r"(dst.m), "r"(m) // input
                : "v0", "v1", "v2", "v3", "memory"
            );
#  else // NEON
            asm volatile
            (
                "vld4.32 {d0[0], d2[0], d4[0], d6[0]}, [%1]!\n\t" // dst.m[0, 4, 8, 12] = m[0-3]
                "vld4.32 {d0[1], d2[1], d4[1], d6[1]}, [%1]!\n\t" // dst.m[1, 5, 9, 13] = m[4-7]
                "vld4.32 {d1[0], d3[0], d5[0], d7[0]}, [%1]!\n\t" // dst.m[2, 6, 10, 14] = m[8-11]
                "vld4.32 {d1[1], d3[1], d5[1], d7[1]}, [%1]\n\t" // dst.m[3, 7, 11, 15] = m[12-15]

                "vst1.32 {q0-q1}, [%0]!\n\t" // dst.m[0-7]
                "vst1.32 {q2-q3}, [%0]\n\t" // dst.m[8-15]
                : // output
                : "r"(dst.m), "r"(m) // input
                : "q0", "q1", "q2", "q3", "memory"
            );
#  endif
#elif defined(__SSE__)
            __m128 tmp0 = _mm_shuffle_ps(_mm_load_ps(&m[0]), _mm_load_ps(&m[4]), _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp2 = _mm_shuffle_ps(_mm_load_ps(&m[0]), _mm_load_ps(&m[4]), _MM_SHUFFLE(3, 2, 3, 2));
            __m128 tmp1 = _mm_shuffle_ps(_mm_load_ps(&m[8]), _mm_load_ps(&m[12]), _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp3 = _mm_shuffle_ps(_mm_load_ps(&m[8]), _mm_load_ps(&m[12]), _MM_SHUFFLE(3, 2, 3, 2));
            _mm_store_ps(&dst.m[0], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)));
            _mm_store_ps(&dst.m[4], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)));
            _mm_store_ps(&dst.m[8], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)));
            _mm_store_ps(&dst.m[12], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1)));
#else
            throw std::runtime_error("Unsupported SIMD architecture");
#endif
        }
        else
        {
            const float t[16] = {
                m[0], m[4], m[8], m[12],
                m[1], m[5], m[9], m[13],
                m[2], m[6], m[10], m[14],
                m[3], m[7], m[11], m[15]
            };
            std::copy(std::begin(t), std::end(t), dst.m);
        }
    }

    template class Matrix<4, 4, float>;
}
