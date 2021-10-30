//
// elnormous/omath
//

#ifndef OMATH_MATRIX_NEON
#define OMATH_MATRIX_NEON

#include "Matrix.hpp"

#ifdef __ARM_NEON__
#  include <arm_neon.h>

namespace omath
{
    template <>
    [[nodiscard]] inline auto operator-(const Matrix<float, 4, 4>& matrix) noexcept
    {
        Matrix<float, 4, 4> result;
        vst1q_f32(&result.m[0], vnegq_f32(vld1q_f32(&matrix.m[0])));
        vst1q_f32(&result.m[4], vnegq_f32(vld1q_f32(&matrix.m[4])));
        vst1q_f32(&result.m[8], vnegq_f32(vld1q_f32(&matrix.m[8])));
        vst1q_f32(&result.m[12], vnegq_f32(vld1q_f32(&matrix.m[12])));
        return result;
    }

    template <>
    inline void negate(Matrix<float, 4, 4>& matrix) noexcept
    {
        vst1q_f32(&matrix.m[0], vnegq_f32(vld1q_f32(&matrix.m[0])));
        vst1q_f32(&matrix.m[4], vnegq_f32(vld1q_f32(&matrix.m[4])));
        vst1q_f32(&matrix.m[8], vnegq_f32(vld1q_f32(&matrix.m[8])));
        vst1q_f32(&matrix.m[12], vnegq_f32(vld1q_f32(&matrix.m[12])));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        vst1q_f32(&result.m[0], vaddq_f32(vld1q_f32(&matrix1.m[0]),
                                          vld1q_f32(&matrix2.m[0])));
        vst1q_f32(&result.m[4], vaddq_f32(vld1q_f32(&matrix1.m[4]),
                                          vld1q_f32(&matrix2.m[4])));
        vst1q_f32(&result.m[8], vaddq_f32(vld1q_f32(&matrix1.m[8]),
                                          vld1q_f32(&matrix2.m[8])));
        vst1q_f32(&result.m[12], vaddq_f32(vld1q_f32(&matrix1.m[12]),
                                           vld1q_f32(&matrix2.m[12])));
        return result;
    }

    template <>
    inline auto& operator+=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        vst1q_f32(&matrix1.m[0], vaddq_f32(vld1q_f32(&matrix1.m[0]),
                                           vld1q_f32(&matrix2.m[0])));
        vst1q_f32(&matrix1.m[4], vaddq_f32(vld1q_f32(&matrix1.m[4]),
                                           vld1q_f32(&matrix2.m[4])));
        vst1q_f32(&matrix1.m[8], vaddq_f32(vld1q_f32(&matrix1.m[8]),
                                           vld1q_f32(&matrix2.m[8])));
        vst1q_f32(&matrix1.m[12], vaddq_f32(vld1q_f32(&matrix1.m[12]),
                                            vld1q_f32(&matrix2.m[12])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        vst1q_f32(&result.m[0], vsubq_f32(vld1q_f32(&matrix1.m[0]),
                                          vld1q_f32(&matrix2.m[0])));
        vst1q_f32(&result.m[4], vsubq_f32(vld1q_f32(&matrix1.m[4]),
                                          vld1q_f32(&matrix2.m[4])));
        vst1q_f32(&result.m[8], vsubq_f32(vld1q_f32(&matrix1.m[8]),
                                          vld1q_f32(&matrix2.m[8])));
        vst1q_f32(&result.m[12], vsubq_f32(vld1q_f32(&matrix1.m[12]),
                                           vld1q_f32(&matrix2.m[12])));
        return result;
    }

    template <>
    inline auto& operator-=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        vst1q_f32(&matrix1.m[0], vsubq_f32(vld1q_f32(&matrix1.m[0]),
                                           vld1q_f32(&matrix2.m[0])));
        vst1q_f32(&matrix1.m[4], vsubq_f32(vld1q_f32(&matrix1.m[4]),
                                           vld1q_f32(&matrix2.m[4])));
        vst1q_f32(&matrix1.m[8], vsubq_f32(vld1q_f32(&matrix1.m[8]),
                                           vld1q_f32(&matrix2.m[8])));
        vst1q_f32(&matrix1.m[12], vsubq_f32(vld1q_f32(&matrix1.m[12]),
                                            vld1q_f32(&matrix2.m[12])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<float, 4, 4>& matrix,
                                        const float scalar) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(&result.m[0], vmulq_f32(vld1q_f32(&matrix.m[0]), s));
        vst1q_f32(&result.m[4], vmulq_f32(vld1q_f32(&matrix.m[4]), s));
        vst1q_f32(&result.m[8], vmulq_f32(vld1q_f32(&matrix.m[8]), s));
        vst1q_f32(&result.m[12], vmulq_f32(vld1q_f32(&matrix.m[12]), s));
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<float, 4, 4>& matrix,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(&matrix.m[0], vmulq_f32(vld1q_f32(&matrix.m[0]), s));
        vst1q_f32(&matrix.m[4], vmulq_f32(vld1q_f32(&matrix.m[4]), s));
        vst1q_f32(&matrix.m[8], vmulq_f32(vld1q_f32(&matrix.m[8]), s));
        vst1q_f32(&matrix.m[12], vmulq_f32(vld1q_f32(&matrix.m[12]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Matrix<float, 4, 4>& matrix,
                                        float scalar) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(&result.m[0], vdivq_f32(vld1q_f32(&matrix.m[0]), s));
        vst1q_f32(&result.m[4], vdivq_f32(vld1q_f32(&matrix.m[4]), s));
        vst1q_f32(&result.m[8], vdivq_f32(vld1q_f32(&matrix.m[8]), s));
        vst1q_f32(&result.m[12], vdivq_f32(vld1q_f32(&matrix.m[12]), s));
        return result;
    }

    template <>
    inline auto& operator/=(Matrix<float, 4, 4>& matrix,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(&matrix.m[0], vdivq_f32(vld1q_f32(&matrix.m[0]), s));
        vst1q_f32(&matrix.m[4], vdivq_f32(vld1q_f32(&matrix.m[4]), s));
        vst1q_f32(&matrix.m[8], vdivq_f32(vld1q_f32(&matrix.m[8]), s));
        vst1q_f32(&matrix.m[12], vdivq_f32(vld1q_f32(&matrix.m[12]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto row0 = vld1q_f32(&matrix1.m[0]);
        const auto row1 = vld1q_f32(&matrix1.m[4]);
        const auto row2 = vld1q_f32(&matrix1.m[8]);
        const auto row3 = vld1q_f32(&matrix1.m[12]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = vdupq_n_f32(matrix2.m[i * 4 + 0]);
            const auto e1 = vdupq_n_f32(matrix2.m[i * 4 + 1]);
            const auto e2 = vdupq_n_f32(matrix2.m[i * 4 + 2]);
            const auto e3 = vdupq_n_f32(matrix2.m[i * 4 + 3]);

            const auto v0 = vmulq_f32(row0, e0);
            const auto v1 = vmulq_f32(row1, e1);
            const auto v2 = vmulq_f32(row2, e2);
            const auto v3 = vmulq_f32(row3, e3);

            const auto a0 = vaddq_f32(v0, v1);
            const auto a1 = vaddq_f32(v2, v3);
            vst1q_f32(&result.m[i * 4], vaddq_f32(a0, a1));
        }
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        const auto row0 = vld1q_f32(&matrix1.m[0]);
        const auto row1 = vld1q_f32(&matrix1.m[4]);
        const auto row2 = vld1q_f32(&matrix1.m[8]);
        const auto row3 = vld1q_f32(&matrix1.m[12]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = vdupq_n_f32(matrix2.m[i * 4 + 0]);
            const auto e1 = vdupq_n_f32(matrix2.m[i * 4 + 1]);
            const auto e2 = vdupq_n_f32(matrix2.m[i * 4 + 2]);
            const auto e3 = vdupq_n_f32(matrix2.m[i * 4 + 3]);

            const auto v0 = vmulq_f32(row0, e0);
            const auto v1 = vmulq_f32(row1, e1);
            const auto v2 = vmulq_f32(row2, e2);
            const auto v3 = vmulq_f32(row3, e3);

            const auto a0 = vaddq_f32(v0, v1);
            const auto a1 = vaddq_f32(v2, v3);
            vst1q_f32(&matrix1.m[i * 4], vaddq_f32(a0, a1));
        }
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const Matrix<float, 4, 4>& matrix) noexcept
    {
        Vector<float, 4> result;

        const auto col0 = vdupq_n_f32(vector.v[0]);
        const auto col1 = vdupq_n_f32(vector.v[1]);
        const auto col2 = vdupq_n_f32(vector.v[2]);
        const auto col3 = vdupq_n_f32(vector.v[3]);

        const auto row0 = vld1q_f32(&matrix.m[0]);
        const auto row1 = vld1q_f32(&matrix.m[4]);
        const auto row2 = vld1q_f32(&matrix.m[8]);
        const auto row3 = vld1q_f32(&matrix.m[12]);

        const auto s = vaddq_f32(vaddq_f32(vmulq_f32(row0, col0),
                                           vmulq_f32(row1, col1)),
                                 vaddq_f32(vmulq_f32(row2, col2),
                                           vmulq_f32(row3, col3)));
        vst1q_f32(result.v.data(), s);

        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto col0 = vdupq_n_f32(vector.v[0]);
        const auto col1 = vdupq_n_f32(vector.v[1]);
        const auto col2 = vdupq_n_f32(vector.v[2]);
        const auto col3 = vdupq_n_f32(vector.v[3]);

        const auto row0 = vld1q_f32(&matrix.m[0]);
        const auto row1 = vld1q_f32(&matrix.m[4]);
        const auto row2 = vld1q_f32(&matrix.m[8]);
        const auto row3 = vld1q_f32(&matrix.m[12]);

        const auto s = vaddq_f32(vaddq_f32(vmulq_f32(row0, col0),
                                           vmulq_f32(row1, col1)),
                                 vaddq_f32(vmulq_f32(row2, col2),
                                           vmulq_f32(row3, col3)));
        vst1q_f32(vector.v.data(), s);

        return vector;
    }

    template <>
    [[nodiscard]] inline auto transposed(const Matrix<float, 4, 4>& matrix) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto tmp0 = vtrnq_f32(vld1q_f32(&matrix.m[0]), vld1q_f32(&matrix.m[4]));
        const auto tmp1 = vtrnq_f32(vld1q_f32(&matrix.m[8]), vld1q_f32(&matrix.m[12]));
        vst1q_f32(&result.m[0], vextq_f32(vextq_f32(tmp0.val[0], tmp0.val[0], 2), tmp1.val[0], 2));
        vst1q_f32(&result.m[4], vextq_f32(vextq_f32(tmp0.val[1], tmp0.val[1], 2), tmp1.val[1], 2));
        vst1q_f32(&result.m[8], vextq_f32(tmp0.val[0], vextq_f32(tmp1.val[0], tmp1.val[0], 2), 2));
        vst1q_f32(&result.m[12], vextq_f32(tmp0.val[1], vextq_f32(tmp1.val[1], tmp1.val[1], 2), 2));
        return result;
    }

    template <>
    inline void transpose(Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto tmp0 = vtrnq_f32(vld1q_f32(&matrix.m[0]), vld1q_f32(&matrix.m[4]));
        const auto tmp1 = vtrnq_f32(vld1q_f32(&matrix.m[8]), vld1q_f32(&matrix.m[12]));
        vst1q_f32(&matrix.m[0], vextq_f32(vextq_f32(tmp0.val[0], tmp0.val[0], 2), tmp1.val[0], 2));
        vst1q_f32(&matrix.m[4], vextq_f32(vextq_f32(tmp0.val[1], tmp0.val[1], 2), tmp1.val[1], 2));
        vst1q_f32(&matrix.m[8], vextq_f32(tmp0.val[0], vextq_f32(tmp1.val[0], tmp1.val[0], 2), 2));
        vst1q_f32(&matrix.m[12], vextq_f32(tmp0.val[1], vextq_f32(tmp1.val[1], tmp1.val[1], 2), 2));
    }

#  ifdef __aarch64__
    template <>
    [[nodiscard]] inline auto operator-(const Matrix<double, 4, 4>& matrix) noexcept
    {
        Matrix<double, 4, 4> result;
        vst1q_f64(&result.m[0], vnegq_f64(vld1q_f64(&matrix.m[0])));
        vst1q_f64(&result.m[2], vnegq_f64(vld1q_f64(&matrix.m[2])));
        vst1q_f64(&result.m[4], vnegq_f64(vld1q_f64(&matrix.m[4])));
        vst1q_f64(&result.m[6], vnegq_f64(vld1q_f64(&matrix.m[6])));
        vst1q_f64(&result.m[8], vnegq_f64(vld1q_f64(&matrix.m[8])));
        vst1q_f64(&result.m[10], vnegq_f64(vld1q_f64(&matrix.m[10])));
        vst1q_f64(&result.m[12], vnegq_f64(vld1q_f64(&matrix.m[12])));
        vst1q_f64(&result.m[14], vnegq_f64(vld1q_f64(&matrix.m[14])));
        return result;
    }

    template <>
    inline void negate(Matrix<double, 4, 4>& matrix) noexcept
    {
        vst1q_f64(&matrix.m[0], vnegq_f64(vld1q_f64(&matrix.m[0])));
        vst1q_f64(&matrix.m[2], vnegq_f64(vld1q_f64(&matrix.m[2])));
        vst1q_f64(&matrix.m[4], vnegq_f64(vld1q_f64(&matrix.m[4])));
        vst1q_f64(&matrix.m[6], vnegq_f64(vld1q_f64(&matrix.m[6])));
        vst1q_f64(&matrix.m[8], vnegq_f64(vld1q_f64(&matrix.m[8])));
        vst1q_f64(&matrix.m[10], vnegq_f64(vld1q_f64(&matrix.m[10])));
        vst1q_f64(&matrix.m[12], vnegq_f64(vld1q_f64(&matrix.m[12])));
        vst1q_f64(&matrix.m[14], vnegq_f64(vld1q_f64(&matrix.m[14])));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        vst1q_f64(&result.m[0], vaddq_f64(vld1q_f64(&matrix1.m[0]),
                                          vld1q_f64(&matrix2.m[0])));
        vst1q_f64(&result.m[2], vaddq_f64(vld1q_f64(&matrix1.m[2]),
                                          vld1q_f64(&matrix2.m[2])));
        vst1q_f64(&result.m[4], vaddq_f64(vld1q_f64(&matrix1.m[4]),
                                          vld1q_f64(&matrix2.m[4])));
        vst1q_f64(&result.m[6], vaddq_f64(vld1q_f64(&matrix1.m[6]),
                                          vld1q_f64(&matrix2.m[6])));
        vst1q_f64(&result.m[8], vaddq_f64(vld1q_f64(&matrix1.m[8]),
                                          vld1q_f64(&matrix2.m[8])));
        vst1q_f64(&result.m[10], vaddq_f64(vld1q_f64(&matrix1.m[10]),
                                           vld1q_f64(&matrix2.m[10])));
        vst1q_f64(&result.m[12], vaddq_f64(vld1q_f64(&matrix1.m[12]),
                                           vld1q_f64(&matrix2.m[12])));
        vst1q_f64(&result.m[14], vaddq_f64(vld1q_f64(&matrix1.m[14]),
                                           vld1q_f64(&matrix2.m[14])));
        return result;
    }

    template <>
    inline auto& operator+=(Matrix<double, 4, 4>& matrix1,
                            const Matrix<double, 4, 4>& matrix2) noexcept
    {
        vst1q_f64(&matrix1.m[0], vaddq_f64(vld1q_f64(&matrix1.m[0]),
                                           vld1q_f64(&matrix2.m[0])));
        vst1q_f64(&matrix1.m[2], vaddq_f64(vld1q_f64(&matrix1.m[2]),
                                           vld1q_f64(&matrix2.m[2])));
        vst1q_f64(&matrix1.m[4], vaddq_f64(vld1q_f64(&matrix1.m[4]),
                                           vld1q_f64(&matrix2.m[4])));
        vst1q_f64(&matrix1.m[6], vaddq_f64(vld1q_f64(&matrix1.m[6]),
                                           vld1q_f64(&matrix2.m[6])));
        vst1q_f64(&matrix1.m[8], vaddq_f64(vld1q_f64(&matrix1.m[8]),
                                           vld1q_f64(&matrix2.m[8])));
        vst1q_f64(&matrix1.m[10], vaddq_f64(vld1q_f64(&matrix1.m[10]),
                                            vld1q_f64(&matrix2.m[10])));
        vst1q_f64(&matrix1.m[12], vaddq_f64(vld1q_f64(&matrix1.m[12]),
                                            vld1q_f64(&matrix2.m[12])));
        vst1q_f64(&matrix1.m[14], vaddq_f64(vld1q_f64(&matrix1.m[14]),
                                            vld1q_f64(&matrix2.m[14])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        vst1q_f64(&result.m[0], vsubq_f64(vld1q_f64(&matrix1.m[0]),
                                          vld1q_f64(&matrix2.m[0])));
        vst1q_f64(&result.m[2], vsubq_f64(vld1q_f64(&matrix1.m[2]),
                                          vld1q_f64(&matrix2.m[2])));
        vst1q_f64(&result.m[4], vsubq_f64(vld1q_f64(&matrix1.m[4]),
                                          vld1q_f64(&matrix2.m[4])));
        vst1q_f64(&result.m[6], vsubq_f64(vld1q_f64(&matrix1.m[6]),
                                          vld1q_f64(&matrix2.m[6])));
        vst1q_f64(&result.m[8], vsubq_f64(vld1q_f64(&matrix1.m[8]),
                                          vld1q_f64(&matrix2.m[8])));
        vst1q_f64(&result.m[10], vsubq_f64(vld1q_f64(&matrix1.m[10]),
                                           vld1q_f64(&matrix2.m[10])));
        vst1q_f64(&result.m[12], vsubq_f64(vld1q_f64(&matrix1.m[12]),
                                           vld1q_f64(&matrix2.m[12])));
        vst1q_f64(&result.m[14], vsubq_f64(vld1q_f64(&matrix1.m[14]),
                                           vld1q_f64(&matrix2.m[14])));
        return result;
    }

    template <>
    inline auto& operator-=(Matrix<double, 4, 4>& matrix1,
                            const Matrix<double, 4, 4>& matrix2) noexcept
    {
        vst1q_f64(&matrix1.m[0], vsubq_f64(vld1q_f64(&matrix1.m[0]),
                                           vld1q_f64(&matrix2.m[0])));
        vst1q_f64(&matrix1.m[2], vsubq_f64(vld1q_f64(&matrix1.m[2]),
                                           vld1q_f64(&matrix2.m[2])));
        vst1q_f64(&matrix1.m[4], vsubq_f64(vld1q_f64(&matrix1.m[4]),
                                           vld1q_f64(&matrix2.m[4])));
        vst1q_f64(&matrix1.m[6], vsubq_f64(vld1q_f64(&matrix1.m[6]),
                                           vld1q_f64(&matrix2.m[6])));
        vst1q_f64(&matrix1.m[8], vsubq_f64(vld1q_f64(&matrix1.m[8]),
                                           vld1q_f64(&matrix2.m[8])));
        vst1q_f64(&matrix1.m[10], vsubq_f64(vld1q_f64(&matrix1.m[10]),
                                            vld1q_f64(&matrix2.m[10])));
        vst1q_f64(&matrix1.m[12], vsubq_f64(vld1q_f64(&matrix1.m[12]),
                                            vld1q_f64(&matrix2.m[12])));
        vst1q_f64(&matrix1.m[14], vsubq_f64(vld1q_f64(&matrix1.m[14]),
                                            vld1q_f64(&matrix2.m[14])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<double, 4, 4>& matrix,
                                        const double scalar) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto s = vdupq_n_f64(scalar);
        vst1q_f64(&result.m[0], vmulq_f64(vld1q_f64(&matrix.m[0]), s));
        vst1q_f64(&result.m[2], vmulq_f64(vld1q_f64(&matrix.m[2]), s));
        vst1q_f64(&result.m[4], vmulq_f64(vld1q_f64(&matrix.m[4]), s));
        vst1q_f64(&result.m[6], vmulq_f64(vld1q_f64(&matrix.m[6]), s));
        vst1q_f64(&result.m[8], vmulq_f64(vld1q_f64(&matrix.m[8]), s));
        vst1q_f64(&result.m[10], vmulq_f64(vld1q_f64(&matrix.m[10]), s));
        vst1q_f64(&result.m[12], vmulq_f64(vld1q_f64(&matrix.m[12]), s));
        vst1q_f64(&result.m[14], vmulq_f64(vld1q_f64(&matrix.m[14]), s));
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<double, 4, 4>& matrix,
                            const double scalar) noexcept
    {
        const auto s = vdupq_n_f64(scalar);
        vst1q_f64(&matrix.m[0], vmulq_f64(vld1q_f64(&matrix.m[0]), s));
        vst1q_f64(&matrix.m[2], vmulq_f64(vld1q_f64(&matrix.m[2]), s));
        vst1q_f64(&matrix.m[4], vmulq_f64(vld1q_f64(&matrix.m[4]), s));
        vst1q_f64(&matrix.m[6], vmulq_f64(vld1q_f64(&matrix.m[6]), s));
        vst1q_f64(&matrix.m[8], vmulq_f64(vld1q_f64(&matrix.m[8]), s));
        vst1q_f64(&matrix.m[10], vmulq_f64(vld1q_f64(&matrix.m[10]), s));
        vst1q_f64(&matrix.m[12], vmulq_f64(vld1q_f64(&matrix.m[12]), s));
        vst1q_f64(&matrix.m[14], vmulq_f64(vld1q_f64(&matrix.m[14]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Matrix<double, 4, 4>& matrix,
                                        double scalar) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto s = vdupq_n_f64(scalar);
        vst1q_f64(&result.m[0], vdivq_f64(vld1q_f64(&matrix.m[0]), s));
        vst1q_f64(&result.m[2], vdivq_f64(vld1q_f64(&matrix.m[2]), s));
        vst1q_f64(&result.m[4], vdivq_f64(vld1q_f64(&matrix.m[4]), s));
        vst1q_f64(&result.m[6], vdivq_f64(vld1q_f64(&matrix.m[6]), s));
        vst1q_f64(&result.m[8], vdivq_f64(vld1q_f64(&matrix.m[8]), s));
        vst1q_f64(&result.m[10], vdivq_f64(vld1q_f64(&matrix.m[10]), s));
        vst1q_f64(&result.m[12], vdivq_f64(vld1q_f64(&matrix.m[12]), s));
        vst1q_f64(&result.m[14], vdivq_f64(vld1q_f64(&matrix.m[14]), s));
        return result;
    }

    template <>
    inline auto& operator/=(Matrix<double, 4, 4>& matrix,
                            const double scalar) noexcept
    {
        const auto s = vdupq_n_f64(scalar);
        vst1q_f64(&matrix.m[0], vdivq_f64(vld1q_f64(&matrix.m[0]), s));
        vst1q_f64(&matrix.m[2], vdivq_f64(vld1q_f64(&matrix.m[2]), s));
        vst1q_f64(&matrix.m[4], vdivq_f64(vld1q_f64(&matrix.m[4]), s));
        vst1q_f64(&matrix.m[6], vdivq_f64(vld1q_f64(&matrix.m[6]), s));
        vst1q_f64(&matrix.m[8], vdivq_f64(vld1q_f64(&matrix.m[8]), s));
        vst1q_f64(&matrix.m[10], vdivq_f64(vld1q_f64(&matrix.m[10]), s));
        vst1q_f64(&matrix.m[12], vdivq_f64(vld1q_f64(&matrix.m[12]), s));
        vst1q_f64(&matrix.m[14], vdivq_f64(vld1q_f64(&matrix.m[14]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto row00 = vld1q_f64(&matrix1.m[0]);
        const auto row01 = vld1q_f64(&matrix1.m[2]);
        const auto row10 = vld1q_f64(&matrix1.m[4]);
        const auto row11 = vld1q_f64(&matrix1.m[6]);
        const auto row20 = vld1q_f64(&matrix1.m[8]);
        const auto row21 = vld1q_f64(&matrix1.m[10]);
        const auto row30 = vld1q_f64(&matrix1.m[12]);
        const auto row31 = vld1q_f64(&matrix1.m[14]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = vdupq_n_f64(matrix2.m[i * 4 + 0]);
            const auto e1 = vdupq_n_f64(matrix2.m[i * 4 + 1]);
            const auto e2 = vdupq_n_f64(matrix2.m[i * 4 + 2]);
            const auto e3 = vdupq_n_f64(matrix2.m[i * 4 + 3]);

            const auto v00 = vmulq_f64(row00, e0);
            const auto v01 = vmulq_f64(row01, e0);
            const auto v10 = vmulq_f64(row10, e1);
            const auto v11 = vmulq_f64(row11, e1);
            const auto v20 = vmulq_f64(row20, e2);
            const auto v21 = vmulq_f64(row21, e2);
            const auto v30 = vmulq_f64(row30, e3);
            const auto v31 = vmulq_f64(row31, e3);

            const auto a00 = vaddq_f64(v00, v10);
            const auto a01 = vaddq_f64(v01, v11);
            const auto a10 = vaddq_f64(v20, v30);
            const auto a11 = vaddq_f64(v21, v31);
            vst1q_f64(&result.m[i * 4 + 0], vaddq_f64(a00, a10));
            vst1q_f64(&result.m[i * 4 + 2], vaddq_f64(a01, a11));
        }
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<double, 4, 4>& matrix1,
                           const Matrix<double, 4, 4>& matrix2) noexcept
    {
        const auto row00 = vld1q_f64(&matrix1.m[0]);
        const auto row01 = vld1q_f64(&matrix1.m[2]);
        const auto row10 = vld1q_f64(&matrix1.m[4]);
        const auto row11 = vld1q_f64(&matrix1.m[6]);
        const auto row20 = vld1q_f64(&matrix1.m[8]);
        const auto row21 = vld1q_f64(&matrix1.m[10]);
        const auto row30 = vld1q_f64(&matrix1.m[12]);
        const auto row31 = vld1q_f64(&matrix1.m[14]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = vdupq_n_f64(matrix2.m[i * 4 + 0]);
            const auto e1 = vdupq_n_f64(matrix2.m[i * 4 + 1]);
            const auto e2 = vdupq_n_f64(matrix2.m[i * 4 + 2]);
            const auto e3 = vdupq_n_f64(matrix2.m[i * 4 + 3]);

            const auto v00 = vmulq_f64(row00, e0);
            const auto v01 = vmulq_f64(row01, e0);
            const auto v10 = vmulq_f64(row10, e1);
            const auto v11 = vmulq_f64(row11, e1);
            const auto v20 = vmulq_f64(row20, e2);
            const auto v21 = vmulq_f64(row21, e2);
            const auto v30 = vmulq_f64(row30, e3);
            const auto v31 = vmulq_f64(row31, e3);

            const auto a00 = vaddq_f64(v00, v10);
            const auto a01 = vaddq_f64(v01, v11);
            const auto a10 = vaddq_f64(v20, v30);
            const auto a11 = vaddq_f64(v21, v31);
            vst1q_f64(&matrix1.m[i * 4 + 0], vaddq_f64(a00, a10));
            vst1q_f64(&matrix1.m[i * 4 + 2], vaddq_f64(a01, a11));
        }
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto transposed(const Matrix<double, 4, 4>& matrix) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto tmp00 = vld1q_f64(&matrix.m[0]);
        const auto tmp01 = vld1q_f64(&matrix.m[2]);
        const auto tmp10 = vld1q_f64(&matrix.m[4]);
        const auto tmp11 = vld1q_f64(&matrix.m[6]);
        const auto tmp20 = vld1q_f64(&matrix.m[8]);
        const auto tmp21 = vld1q_f64(&matrix.m[10]);
        const auto tmp30 = vld1q_f64(&matrix.m[12]);
        const auto tmp31 = vld1q_f64(&matrix.m[14]);

        vst1q_f64(&result.m[0], vtrn1q_f64(tmp00, tmp10));
        vst1q_f64(&result.m[2], vtrn1q_f64(tmp20, tmp30));
        vst1q_f64(&result.m[4], vtrn2q_f64(tmp00, tmp10));
        vst1q_f64(&result.m[6], vtrn2q_f64(tmp20, tmp30));
        vst1q_f64(&result.m[8], vtrn1q_f64(tmp01, tmp11));
        vst1q_f64(&result.m[10], vtrn1q_f64(tmp21, tmp31));
        vst1q_f64(&result.m[12], vtrn2q_f64(tmp01, tmp11));
        vst1q_f64(&result.m[14], vtrn2q_f64(tmp21, tmp31));
        return result;
    }

    template <>
    inline void transpose(Matrix<double, 4, 4>& matrix) noexcept
    {
        const auto tmp00 = vld1q_f64(&matrix.m[0]);
        const auto tmp01 = vld1q_f64(&matrix.m[2]);
        const auto tmp10 = vld1q_f64(&matrix.m[4]);
        const auto tmp11 = vld1q_f64(&matrix.m[6]);
        const auto tmp20 = vld1q_f64(&matrix.m[8]);
        const auto tmp21 = vld1q_f64(&matrix.m[10]);
        const auto tmp30 = vld1q_f64(&matrix.m[12]);
        const auto tmp31 = vld1q_f64(&matrix.m[14]);

        vst1q_f64(&matrix.m[0], vtrn1q_f64(tmp00, tmp10));
        vst1q_f64(&matrix.m[2], vtrn1q_f64(tmp20, tmp30));
        vst1q_f64(&matrix.m[4], vtrn2q_f64(tmp00, tmp10));
        vst1q_f64(&matrix.m[6], vtrn2q_f64(tmp20, tmp30));
        vst1q_f64(&matrix.m[8], vtrn1q_f64(tmp01, tmp11));
        vst1q_f64(&matrix.m[10], vtrn1q_f64(tmp21, tmp31));
        vst1q_f64(&matrix.m[12], vtrn2q_f64(tmp01, tmp11));
        vst1q_f64(&matrix.m[14], vtrn2q_f64(tmp21, tmp31));
    }
#  endif // __aarch64__
}

#endif // __ARM_NEON__

#endif // OMATH_MATRIX_NEON
