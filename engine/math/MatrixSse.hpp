// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_MATRIX_SSE_HPP
#define OUZEL_MATH_MATRIX_SSE_HPP

#include "Matrix.hpp"

#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
#  include <xmmintrin.h>
#endif

#if defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2
#  include <emmintrin.h>
#endif

namespace ouzel::math
{
#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
#  ifndef __AVX__
    template <>
    [[nodiscard]] inline auto operator-(const Matrix<float, 4, 4>& matrix) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto z = _mm_setzero_ps();
        _mm_store_ps(&result.m.v[0], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[0])));
        _mm_store_ps(&result.m.v[4], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[4])));
        _mm_store_ps(&result.m.v[8], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[8])));
        _mm_store_ps(&result.m.v[12], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[12])));
        return result;
    }

    template <>
    inline void negate(Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto z = _mm_setzero_ps();
        _mm_store_ps(&matrix.m.v[0], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[0])));
        _mm_store_ps(&matrix.m.v[4], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[4])));
        _mm_store_ps(&matrix.m.v[8], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[8])));
        _mm_store_ps(&matrix.m.v[12], _mm_sub_ps(z, _mm_load_ps(&matrix.m.v[12])));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        _mm_store_ps(&result.m.v[0], _mm_add_ps(_mm_load_ps(&matrix1.m.v[0]),
                                                _mm_load_ps(&matrix2.m.v[0])));
        _mm_store_ps(&result.m.v[4], _mm_add_ps(_mm_load_ps(&matrix1.m.v[4]),
                                                _mm_load_ps(&matrix2.m.v[4])));
        _mm_store_ps(&result.m.v[8], _mm_add_ps(_mm_load_ps(&matrix1.m.v[8]),
                                                _mm_load_ps(&matrix2.m.v[8])));
        _mm_store_ps(&result.m.v[12], _mm_add_ps(_mm_load_ps(&matrix1.m.v[12]),
                                                 _mm_load_ps(&matrix2.m.v[12])));
        return result;
    }

    template <>
    inline auto& operator+=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        _mm_store_ps(&matrix1.m.v[0], _mm_add_ps(_mm_load_ps(&matrix1.m.v[0]),
                                                 _mm_load_ps(&matrix2.m.v[0])));
        _mm_store_ps(&matrix1.m.v[4], _mm_add_ps(_mm_load_ps(&matrix1.m.v[4]),
                                                 _mm_load_ps(&matrix2.m.v[4])));
        _mm_store_ps(&matrix1.m.v[8], _mm_add_ps(_mm_load_ps(&matrix1.m.v[8]),
                                                 _mm_load_ps(&matrix2.m.v[8])));
        _mm_store_ps(&matrix1.m.v[12], _mm_add_ps(_mm_load_ps(&matrix1.m.v[12]),
                                                  _mm_load_ps(&matrix2.m.v[12])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        _mm_store_ps(&result.m.v[0], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[0]),
                                                _mm_load_ps(&matrix2.m.v[0])));
        _mm_store_ps(&result.m.v[4], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[4]),
                                                _mm_load_ps(&matrix2.m.v[4])));
        _mm_store_ps(&result.m.v[8], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[8]),
                                                _mm_load_ps(&matrix2.m.v[8])));
        _mm_store_ps(&result.m.v[12], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[12]),
                                                 _mm_load_ps(&matrix2.m.v[12])));
        return result;
    }

    template <>
    inline auto& operator-=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        _mm_store_ps(&matrix1.m.v[0], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[0]),
                                                 _mm_load_ps(&matrix2.m.v[0])));
        _mm_store_ps(&matrix1.m.v[4], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[4]),
                                                 _mm_load_ps(&matrix2.m.v[4])));
        _mm_store_ps(&matrix1.m.v[8], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[8]),
                                                 _mm_load_ps(&matrix2.m.v[8])));
        _mm_store_ps(&matrix1.m.v[12], _mm_sub_ps(_mm_load_ps(&matrix1.m.v[12]),
                                                  _mm_load_ps(&matrix2.m.v[12])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<float, 4, 4>& matrix,
                                        const float scalar) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(&result.m.v[0], _mm_mul_ps(_mm_load_ps(&matrix.m.v[0]), s));
        _mm_store_ps(&result.m.v[4], _mm_mul_ps(_mm_load_ps(&matrix.m.v[4]), s));
        _mm_store_ps(&result.m.v[8], _mm_mul_ps(_mm_load_ps(&matrix.m.v[8]), s));
        _mm_store_ps(&result.m.v[12], _mm_mul_ps(_mm_load_ps(&matrix.m.v[12]), s));
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<float, 4, 4>& matrix,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(&matrix.m.v[0], _mm_mul_ps(_mm_load_ps(&matrix.m.v[0]), s));
        _mm_store_ps(&matrix.m.v[4], _mm_mul_ps(_mm_load_ps(&matrix.m.v[4]), s));
        _mm_store_ps(&matrix.m.v[8], _mm_mul_ps(_mm_load_ps(&matrix.m.v[8]), s));
        _mm_store_ps(&matrix.m.v[12], _mm_mul_ps(_mm_load_ps(&matrix.m.v[12]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Matrix<float, 4, 4>& matrix,
                                        float scalar) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(&result.m.v[0], _mm_div_ps(_mm_load_ps(&matrix.m.v[0]), s));
        _mm_store_ps(&result.m.v[4], _mm_div_ps(_mm_load_ps(&matrix.m.v[4]), s));
        _mm_store_ps(&result.m.v[8], _mm_div_ps(_mm_load_ps(&matrix.m.v[8]), s));
        _mm_store_ps(&result.m.v[12], _mm_div_ps(_mm_load_ps(&matrix.m.v[12]), s));
        return result;
    }

    template <>
    inline auto& operator/=(Matrix<float, 4, 4>& matrix,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(&matrix.m.v[0], _mm_div_ps(_mm_load_ps(&matrix.m.v[0]), s));
        _mm_store_ps(&matrix.m.v[4], _mm_div_ps(_mm_load_ps(&matrix.m.v[4]), s));
        _mm_store_ps(&matrix.m.v[8], _mm_div_ps(_mm_load_ps(&matrix.m.v[8]), s));
        _mm_store_ps(&matrix.m.v[12], _mm_div_ps(_mm_load_ps(&matrix.m.v[12]), s));
        return matrix;
    }
#  endif // __AVX__

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<float, 4, 4>& matrix1,
                                        const Matrix<float, 4, 4>& matrix2) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto col0 = _mm_load_ps(&matrix1.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix1.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix1.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix1.m.v[12]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = _mm_set1_ps(matrix2.m.v[i * 4 + 0]);
            const auto e1 = _mm_set1_ps(matrix2.m.v[i * 4 + 1]);
            const auto e2 = _mm_set1_ps(matrix2.m.v[i * 4 + 2]);
            const auto e3 = _mm_set1_ps(matrix2.m.v[i * 4 + 3]);

            const auto v0 = _mm_mul_ps(col0, e0);
            const auto v1 = _mm_mul_ps(col1, e1);
            const auto v2 = _mm_mul_ps(col2, e2);
            const auto v3 = _mm_mul_ps(col3, e3);

            const auto a0 = _mm_add_ps(v0, v1);
            const auto a1 = _mm_add_ps(v2, v3);
            _mm_store_ps(&result.m.v[i * 4], _mm_add_ps(a0, a1));
        }
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<float, 4, 4>& matrix1,
                            const Matrix<float, 4, 4>& matrix2) noexcept
    {
        const auto col0 = _mm_load_ps(&matrix1.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix1.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix1.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix1.m.v[12]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = _mm_set1_ps(matrix2.m.v[i * 4 + 0]);
            const auto e1 = _mm_set1_ps(matrix2.m.v[i * 4 + 1]);
            const auto e2 = _mm_set1_ps(matrix2.m.v[i * 4 + 2]);
            const auto e3 = _mm_set1_ps(matrix2.m.v[i * 4 + 3]);

            const auto v0 = _mm_mul_ps(col0, e0);
            const auto v1 = _mm_mul_ps(col1, e1);
            const auto v2 = _mm_mul_ps(col2, e2);
            const auto v3 = _mm_mul_ps(col3, e3);

            const auto a0 = _mm_add_ps(v0, v1);
            const auto a1 = _mm_add_ps(v2, v3);
            _mm_store_ps(&matrix1.m.v[i * 4], _mm_add_ps(a0, a1));
        }
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<float, 4, 4>& matrix,
                                        const Vector<float, 4>& vector) noexcept
    {
        Vector<float, 4> result;

        const auto row0 = _mm_set1_ps(vector.v[0]);
        const auto row1 = _mm_set1_ps(vector.v[1]);
        const auto row2 = _mm_set1_ps(vector.v[2]);
        const auto row3 = _mm_set1_ps(vector.v[3]);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto s = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row0, col0),
                                             _mm_mul_ps(row1, col1)),
                                  _mm_add_ps(_mm_mul_ps(row2, col2),
                                             _mm_mul_ps(row3, col3)));
        _mm_store_ps(result.v, s);

        return result;
    }

    template <>
    inline void transformVector(const Matrix<float, 4, 4>& matrix,
                                Vector<float, 4>& vector) noexcept
    {
        const auto row0 = _mm_set1_ps(vector.v[0]);
        const auto row1 = _mm_set1_ps(vector.v[1]);
        const auto row2 = _mm_set1_ps(vector.v[2]);
        const auto row3 = _mm_set1_ps(vector.v[3]);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto s = _mm_add_ps(_mm_add_ps(_mm_mul_ps(row0, col0),
                                             _mm_mul_ps(row1, col1)),
                                  _mm_add_ps(_mm_mul_ps(row2, col2),
                                             _mm_mul_ps(row3, col3)));
        _mm_store_ps(vector.v, s);
    }

#  ifndef __SSE3__
    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const Matrix<float, 4, 4>& matrix) noexcept
    {
        Vector<float, 4> result;

        const auto v = _mm_load_ps(vector.v);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto m0 = _mm_mul_ps(v, col0);
        const auto m1 = _mm_mul_ps(v, col1);
        const auto m2 = _mm_mul_ps(v, col2);
        const auto m3 = _mm_mul_ps(v, col3);

        const auto a0 = _mm_add_ps(_mm_unpacklo_ps(m0, m1), _mm_unpackhi_ps(m0, m1));
        const auto a1 = _mm_add_ps(_mm_unpacklo_ps(m2, m3), _mm_unpackhi_ps(m2, m3));
        const auto a3 = _mm_add_ps(_mm_movelh_ps(a0, a1), _mm_movehl_ps(a1, a0));

        _mm_store_ps(result.v, a3);

        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto v = _mm_load_ps(vector.v);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto m0 = _mm_mul_ps(v, col0);
        const auto m1 = _mm_mul_ps(v, col1);
        const auto m2 = _mm_mul_ps(v, col2);
        const auto m3 = _mm_mul_ps(v, col3);

        const auto a0 = _mm_add_ps(_mm_unpacklo_ps(m0, m1), _mm_unpackhi_ps(m0, m1));
        const auto a1 = _mm_add_ps(_mm_unpacklo_ps(m2, m3), _mm_unpackhi_ps(m2, m3));
        const auto a3 = _mm_add_ps(_mm_movelh_ps(a0, a1), _mm_movehl_ps(a1, a0));

        _mm_store_ps(vector.v, a3);

        return vector;
    }
#  endif // SSE3

    template <>
    [[nodiscard]] inline auto transposed(const Matrix<float, 4, 4>& matrix) noexcept
    {
        Matrix<float, 4, 4> result;
        const auto tmp0 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[0]),
                                         _mm_load_ps(&matrix.m.v[4]),
                                         _MM_SHUFFLE(1, 0, 1, 0));
        const auto tmp1 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[8]),
                                         _mm_load_ps(&matrix.m.v[12]),
                                         _MM_SHUFFLE(1, 0, 1, 0));
        const auto tmp2 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[0]),
                                         _mm_load_ps(&matrix.m.v[4]),
                                         _MM_SHUFFLE(3, 2, 3, 2));
        const auto tmp3 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[8]),
                                         _mm_load_ps(&matrix.m.v[12]),
                                         _MM_SHUFFLE(3, 2, 3, 2));
        _mm_store_ps(&result.m.v[0], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)));
        _mm_store_ps(&result.m.v[4], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)));
        _mm_store_ps(&result.m.v[8], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)));
        _mm_store_ps(&result.m.v[12], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1)));
        return result;
    }

    template <>
    inline void transpose(Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto tmp0 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[0]),
                                         _mm_load_ps(&matrix.m.v[4]),
                                         _MM_SHUFFLE(1, 0, 1, 0));
        const auto tmp1 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[8]),
                                         _mm_load_ps(&matrix.m.v[12]),
                                         _MM_SHUFFLE(1, 0, 1, 0));
        const auto tmp2 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[0]),
                                         _mm_load_ps(&matrix.m.v[4]),
                                         _MM_SHUFFLE(3, 2, 3, 2));
        const auto tmp3 = _mm_shuffle_ps(_mm_load_ps(&matrix.m.v[8]),
                                         _mm_load_ps(&matrix.m.v[12]),
                                         _MM_SHUFFLE(3, 2, 3, 2));
        _mm_store_ps(&matrix.m.v[0], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)));
        _mm_store_ps(&matrix.m.v[4], _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)));
        _mm_store_ps(&matrix.m.v[8], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)));
        _mm_store_ps(&matrix.m.v[12], _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1)));
    }
#endif // SSE

#if defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2
#  ifndef __AVX__
    template <>
    [[nodiscard]] inline auto operator-(const Matrix<double, 4, 4>& matrix) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto z = _mm_setzero_pd();
        _mm_store_pd(&result.m.v[0], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[0])));
        _mm_store_pd(&result.m.v[2], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[2])));
        _mm_store_pd(&result.m.v[4], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[4])));
        _mm_store_pd(&result.m.v[6], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[6])));
        _mm_store_pd(&result.m.v[8], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[8])));
        _mm_store_pd(&result.m.v[10], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[10])));
        _mm_store_pd(&result.m.v[12], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[12])));
        _mm_store_pd(&result.m.v[14], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[14])));
        return result;
    }

    template <>
    inline void negate(Matrix<double, 4, 4>& matrix) noexcept
    {
        const auto z = _mm_setzero_pd();
        _mm_store_pd(&matrix.m.v[0], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[0])));
        _mm_store_pd(&matrix.m.v[2], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[2])));
        _mm_store_pd(&matrix.m.v[4], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[4])));
        _mm_store_pd(&matrix.m.v[6], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[6])));
        _mm_store_pd(&matrix.m.v[8], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[8])));
        _mm_store_pd(&matrix.m.v[10], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[10])));
        _mm_store_pd(&matrix.m.v[12], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[12])));
        _mm_store_pd(&matrix.m.v[14], _mm_sub_pd(z, _mm_load_pd(&matrix.m.v[14])));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        _mm_store_pd(&result.m.v[0], _mm_add_pd(_mm_load_pd(&matrix1.m.v[0]),
                                                _mm_load_pd(&matrix2.m.v[0])));
        _mm_store_pd(&result.m.v[2], _mm_add_pd(_mm_load_pd(&matrix1.m.v[2]),
                                                _mm_load_pd(&matrix2.m.v[2])));
        _mm_store_pd(&result.m.v[4], _mm_add_pd(_mm_load_pd(&matrix1.m.v[4]),
                                                _mm_load_pd(&matrix2.m.v[4])));
        _mm_store_pd(&result.m.v[6], _mm_add_pd(_mm_load_pd(&matrix1.m.v[6]),
                                                _mm_load_pd(&matrix2.m.v[6])));
        _mm_store_pd(&result.m.v[8], _mm_add_pd(_mm_load_pd(&matrix1.m.v[8]),
                                                _mm_load_pd(&matrix2.m.v[8])));
        _mm_store_pd(&result.m.v[10], _mm_add_pd(_mm_load_pd(&matrix1.m.v[10]),
                                                 _mm_load_pd(&matrix2.m.v[10])));
        _mm_store_pd(&result.m.v[12], _mm_add_pd(_mm_load_pd(&matrix1.m.v[12]),
                                                 _mm_load_pd(&matrix2.m.v[12])));
        _mm_store_pd(&result.m.v[14], _mm_add_pd(_mm_load_pd(&matrix1.m.v[14]),
                                                 _mm_load_pd(&matrix2.m.v[14])));
        return result;
    }

    template <>
    inline auto& operator+=(Matrix<double, 4, 4>& matrix1,
                            const Matrix<double, 4, 4>& matrix2) noexcept
    {
        _mm_store_pd(&matrix1.m.v[0], _mm_add_pd(_mm_load_pd(&matrix1.m.v[0]),
                                                 _mm_load_pd(&matrix2.m.v[0])));
        _mm_store_pd(&matrix1.m.v[2], _mm_add_pd(_mm_load_pd(&matrix1.m.v[2]),
                                                 _mm_load_pd(&matrix2.m.v[2])));
        _mm_store_pd(&matrix1.m.v[4], _mm_add_pd(_mm_load_pd(&matrix1.m.v[4]),
                                                 _mm_load_pd(&matrix2.m.v[4])));
        _mm_store_pd(&matrix1.m.v[6], _mm_add_pd(_mm_load_pd(&matrix1.m.v[6]),
                                                 _mm_load_pd(&matrix2.m.v[6])));
        _mm_store_pd(&matrix1.m.v[8], _mm_add_pd(_mm_load_pd(&matrix1.m.v[8]),
                                                 _mm_load_pd(&matrix2.m.v[8])));
        _mm_store_pd(&matrix1.m.v[10], _mm_add_pd(_mm_load_pd(&matrix1.m.v[10]),
                                                  _mm_load_pd(&matrix2.m.v[10])));
        _mm_store_pd(&matrix1.m.v[12], _mm_add_pd(_mm_load_pd(&matrix1.m.v[12]),
                                                  _mm_load_pd(&matrix2.m.v[12])));
        _mm_store_pd(&matrix1.m.v[14], _mm_add_pd(_mm_load_pd(&matrix1.m.v[14]),
                                                  _mm_load_pd(&matrix2.m.v[14])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        _mm_store_pd(&result.m.v[0], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[0]),
                                                _mm_load_pd(&matrix2.m.v[0])));
        _mm_store_pd(&result.m.v[2], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[2]),
                                                _mm_load_pd(&matrix2.m.v[2])));
        _mm_store_pd(&result.m.v[4], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[4]),
                                                _mm_load_pd(&matrix2.m.v[4])));
        _mm_store_pd(&result.m.v[6], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[6]),
                                                _mm_load_pd(&matrix2.m.v[6])));
        _mm_store_pd(&result.m.v[8], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[8]),
                                                _mm_load_pd(&matrix2.m.v[8])));
        _mm_store_pd(&result.m.v[10], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[10]),
                                                 _mm_load_pd(&matrix2.m.v[10])));
        _mm_store_pd(&result.m.v[12], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[12]),
                                                 _mm_load_pd(&matrix2.m.v[12])));
        _mm_store_pd(&result.m.v[14], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[14]),
                                                 _mm_load_pd(&matrix2.m.v[14])));
        return result;
    }

    template <>
    inline auto& operator-=(Matrix<double, 4, 4>& matrix1,
                            const Matrix<double, 4, 4>& matrix2) noexcept
    {
        _mm_store_pd(&matrix1.m.v[0], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[0]),
                                                 _mm_load_pd(&matrix2.m.v[0])));
        _mm_store_pd(&matrix1.m.v[2], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[2]),
                                                 _mm_load_pd(&matrix2.m.v[2])));
        _mm_store_pd(&matrix1.m.v[4], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[4]),
                                                 _mm_load_pd(&matrix2.m.v[4])));
        _mm_store_pd(&matrix1.m.v[6], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[6]),
                                                 _mm_load_pd(&matrix2.m.v[6])));
        _mm_store_pd(&matrix1.m.v[8], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[8]),
                                                 _mm_load_pd(&matrix2.m.v[8])));
        _mm_store_pd(&matrix1.m.v[10], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[10]),
                                                  _mm_load_pd(&matrix2.m.v[10])));
        _mm_store_pd(&matrix1.m.v[12], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[12]),
                                                  _mm_load_pd(&matrix2.m.v[12])));
        _mm_store_pd(&matrix1.m.v[14], _mm_sub_pd(_mm_load_pd(&matrix1.m.v[14]),
                                                  _mm_load_pd(&matrix2.m.v[14])));
        return matrix1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<double, 4, 4>& matrix,
                                        const double scalar) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&result.m.v[0], _mm_mul_pd(_mm_load_pd(&matrix.m.v[0]), s));
        _mm_store_pd(&result.m.v[2], _mm_mul_pd(_mm_load_pd(&matrix.m.v[2]), s));
        _mm_store_pd(&result.m.v[4], _mm_mul_pd(_mm_load_pd(&matrix.m.v[4]), s));
        _mm_store_pd(&result.m.v[6], _mm_mul_pd(_mm_load_pd(&matrix.m.v[6]), s));
        _mm_store_pd(&result.m.v[8], _mm_mul_pd(_mm_load_pd(&matrix.m.v[8]), s));
        _mm_store_pd(&result.m.v[10], _mm_mul_pd(_mm_load_pd(&matrix.m.v[10]), s));
        _mm_store_pd(&result.m.v[12], _mm_mul_pd(_mm_load_pd(&matrix.m.v[12]), s));
        _mm_store_pd(&result.m.v[14], _mm_mul_pd(_mm_load_pd(&matrix.m.v[14]), s));
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<double, 4, 4>& matrix,
                            const double scalar) noexcept
    {
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&matrix.m.v[0], _mm_mul_pd(_mm_load_pd(&matrix.m.v[0]), s));
        _mm_store_pd(&matrix.m.v[2], _mm_mul_pd(_mm_load_pd(&matrix.m.v[2]), s));
        _mm_store_pd(&matrix.m.v[4], _mm_mul_pd(_mm_load_pd(&matrix.m.v[4]), s));
        _mm_store_pd(&matrix.m.v[6], _mm_mul_pd(_mm_load_pd(&matrix.m.v[6]), s));
        _mm_store_pd(&matrix.m.v[8], _mm_mul_pd(_mm_load_pd(&matrix.m.v[8]), s));
        _mm_store_pd(&matrix.m.v[10], _mm_mul_pd(_mm_load_pd(&matrix.m.v[10]), s));
        _mm_store_pd(&matrix.m.v[12], _mm_mul_pd(_mm_load_pd(&matrix.m.v[12]), s));
        _mm_store_pd(&matrix.m.v[14], _mm_mul_pd(_mm_load_pd(&matrix.m.v[14]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Matrix<double, 4, 4>& matrix,
                                        double scalar) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&result.m.v[0], _mm_div_pd(_mm_load_pd(&matrix.m.v[0]), s));
        _mm_store_pd(&result.m.v[2], _mm_div_pd(_mm_load_pd(&matrix.m.v[2]), s));
        _mm_store_pd(&result.m.v[4], _mm_div_pd(_mm_load_pd(&matrix.m.v[4]), s));
        _mm_store_pd(&result.m.v[6], _mm_div_pd(_mm_load_pd(&matrix.m.v[6]), s));
        _mm_store_pd(&result.m.v[8], _mm_div_pd(_mm_load_pd(&matrix.m.v[8]), s));
        _mm_store_pd(&result.m.v[10], _mm_div_pd(_mm_load_pd(&matrix.m.v[10]), s));
        _mm_store_pd(&result.m.v[12], _mm_div_pd(_mm_load_pd(&matrix.m.v[12]), s));
        _mm_store_pd(&result.m.v[14], _mm_div_pd(_mm_load_pd(&matrix.m.v[14]), s));
        return result;
    }

    template <>
    inline auto& operator/=(Matrix<double, 4, 4>& matrix,
                            const double scalar) noexcept
    {
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&matrix.m.v[0], _mm_div_pd(_mm_load_pd(&matrix.m.v[0]), s));
        _mm_store_pd(&matrix.m.v[2], _mm_div_pd(_mm_load_pd(&matrix.m.v[2]), s));
        _mm_store_pd(&matrix.m.v[4], _mm_div_pd(_mm_load_pd(&matrix.m.v[4]), s));
        _mm_store_pd(&matrix.m.v[6], _mm_div_pd(_mm_load_pd(&matrix.m.v[6]), s));
        _mm_store_pd(&matrix.m.v[8], _mm_div_pd(_mm_load_pd(&matrix.m.v[8]), s));
        _mm_store_pd(&matrix.m.v[10], _mm_div_pd(_mm_load_pd(&matrix.m.v[10]), s));
        _mm_store_pd(&matrix.m.v[12], _mm_div_pd(_mm_load_pd(&matrix.m.v[12]), s));
        _mm_store_pd(&matrix.m.v[14], _mm_div_pd(_mm_load_pd(&matrix.m.v[14]), s));
        return matrix;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Matrix<double, 4, 4>& matrix1,
                                        const Matrix<double, 4, 4>& matrix2) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto col00 = _mm_load_pd(&matrix1.m.v[0]);
        const auto col01 = _mm_load_pd(&matrix1.m.v[2]);
        const auto col10 = _mm_load_pd(&matrix1.m.v[4]);
        const auto col11 = _mm_load_pd(&matrix1.m.v[6]);
        const auto col20 = _mm_load_pd(&matrix1.m.v[8]);
        const auto col21 = _mm_load_pd(&matrix1.m.v[10]);
        const auto col30 = _mm_load_pd(&matrix1.m.v[12]);
        const auto col31 = _mm_load_pd(&matrix1.m.v[14]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = _mm_set1_pd(matrix2.m.v[i * 4 + 0]);
            const auto e1 = _mm_set1_pd(matrix2.m.v[i * 4 + 1]);
            const auto e2 = _mm_set1_pd(matrix2.m.v[i * 4 + 2]);
            const auto e3 = _mm_set1_pd(matrix2.m.v[i * 4 + 3]);

            const auto v00 = _mm_mul_pd(col00, e0);
            const auto v01 = _mm_mul_pd(col01, e0);
            const auto v10 = _mm_mul_pd(col10, e1);
            const auto v11 = _mm_mul_pd(col11, e1);
            const auto v20 = _mm_mul_pd(col20, e2);
            const auto v21 = _mm_mul_pd(col21, e2);
            const auto v30 = _mm_mul_pd(col30, e3);
            const auto v31 = _mm_mul_pd(col31, e3);

            const auto a00 = _mm_add_pd(v00, v10);
            const auto a01 = _mm_add_pd(v01, v11);
            const auto a10 = _mm_add_pd(v20, v30);
            const auto a11 = _mm_add_pd(v21, v31);
            _mm_store_pd(&result.m.v[i * 4 + 0], _mm_add_pd(a00, a10));
            _mm_store_pd(&result.m.v[i * 4 + 2], _mm_add_pd(a01, a11));
        }
        return result;
    }

    template <>
    inline auto& operator*=(Matrix<double, 4, 4>& matrix1,
                            const Matrix<double, 4, 4>& matrix2) noexcept
    {
        const auto col00 = _mm_load_pd(&matrix1.m.v[0]);
        const auto col01 = _mm_load_pd(&matrix1.m.v[2]);
        const auto col10 = _mm_load_pd(&matrix1.m.v[4]);
        const auto col11 = _mm_load_pd(&matrix1.m.v[6]);
        const auto col20 = _mm_load_pd(&matrix1.m.v[8]);
        const auto col21 = _mm_load_pd(&matrix1.m.v[10]);
        const auto col30 = _mm_load_pd(&matrix1.m.v[12]);
        const auto col31 = _mm_load_pd(&matrix1.m.v[14]);

        for (std::size_t i = 0; i < 4; ++i)
        {
            const auto e0 = _mm_set1_pd(matrix2.m.v[i * 4 + 0]);
            const auto e1 = _mm_set1_pd(matrix2.m.v[i * 4 + 1]);
            const auto e2 = _mm_set1_pd(matrix2.m.v[i * 4 + 2]);
            const auto e3 = _mm_set1_pd(matrix2.m.v[i * 4 + 3]);

            const auto v00 = _mm_mul_pd(col00, e0);
            const auto v01 = _mm_mul_pd(col01, e0);
            const auto v10 = _mm_mul_pd(col10, e1);
            const auto v11 = _mm_mul_pd(col11, e1);
            const auto v20 = _mm_mul_pd(col20, e2);
            const auto v21 = _mm_mul_pd(col21, e2);
            const auto v30 = _mm_mul_pd(col30, e3);
            const auto v31 = _mm_mul_pd(col31, e3);

            const auto a00 = _mm_add_pd(v00, v10);
            const auto a01 = _mm_add_pd(v01, v11);
            const auto a10 = _mm_add_pd(v20, v30);
            const auto a11 = _mm_add_pd(v21, v31);
            _mm_store_pd(&matrix1.m.v[i * 4 + 0], _mm_add_pd(a00, a10));
            _mm_store_pd(&matrix1.m.v[i * 4 + 2], _mm_add_pd(a01, a11));
        }
        return matrix1;
    }
#  endif // __AVX__

    template <>
    [[nodiscard]] inline auto transposed(const Matrix<double, 4, 4>& matrix) noexcept
    {
        Matrix<double, 4, 4> result;
        const auto tmp00 = _mm_load_pd(&matrix.m.v[0]);
        const auto tmp01 = _mm_load_pd(&matrix.m.v[2]);
        const auto tmp10 = _mm_load_pd(&matrix.m.v[4]);
        const auto tmp11 = _mm_load_pd(&matrix.m.v[6]);
        const auto tmp20 = _mm_load_pd(&matrix.m.v[8]);
        const auto tmp21 = _mm_load_pd(&matrix.m.v[10]);
        const auto tmp30 = _mm_load_pd(&matrix.m.v[12]);
        const auto tmp31 = _mm_load_pd(&matrix.m.v[14]);

        _mm_store_pd(&result.m.v[0], _mm_shuffle_pd(tmp00, tmp10, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&result.m.v[2], _mm_shuffle_pd(tmp20, tmp30, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&result.m.v[4], _mm_shuffle_pd(tmp00, tmp10, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&result.m.v[6], _mm_shuffle_pd(tmp20, tmp30, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&result.m.v[8], _mm_shuffle_pd(tmp01, tmp11, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&result.m.v[10], _mm_shuffle_pd(tmp21, tmp31, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&result.m.v[12], _mm_shuffle_pd(tmp01, tmp11, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&result.m.v[14], _mm_shuffle_pd(tmp21, tmp31, _MM_SHUFFLE2(1, 1)));
        return result;
    }

    template <>
    inline void transpose(Matrix<double, 4, 4>& matrix) noexcept
    {
        const auto tmp00 = _mm_load_pd(&matrix.m.v[0]);
        const auto tmp01 = _mm_load_pd(&matrix.m.v[2]);
        const auto tmp10 = _mm_load_pd(&matrix.m.v[4]);
        const auto tmp11 = _mm_load_pd(&matrix.m.v[6]);
        const auto tmp20 = _mm_load_pd(&matrix.m.v[8]);
        const auto tmp21 = _mm_load_pd(&matrix.m.v[10]);
        const auto tmp30 = _mm_load_pd(&matrix.m.v[12]);
        const auto tmp31 = _mm_load_pd(&matrix.m.v[14]);

        _mm_store_pd(&matrix.m.v[0], _mm_shuffle_pd(tmp00, tmp10, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&matrix.m.v[2], _mm_shuffle_pd(tmp20, tmp30, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&matrix.m.v[4], _mm_shuffle_pd(tmp00, tmp10, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&matrix.m.v[6], _mm_shuffle_pd(tmp20, tmp30, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&matrix.m.v[8], _mm_shuffle_pd(tmp01, tmp11, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&matrix.m.v[10], _mm_shuffle_pd(tmp21, tmp31, _MM_SHUFFLE2(0, 0)));
        _mm_store_pd(&matrix.m.v[12], _mm_shuffle_pd(tmp01, tmp11, _MM_SHUFFLE2(1, 1)));
        _mm_store_pd(&matrix.m.v[14], _mm_shuffle_pd(tmp21, tmp31, _MM_SHUFFLE2(1, 1)));
    }
#endif // SSE2

#ifdef __SSE3__
    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const Matrix<float, 4, 4>& matrix) noexcept
    {
        Vector<float, 4> result;

        const auto v = _mm_load_ps(vector.v);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto m0 = _mm_mul_ps(v, col0);
        const auto m1 = _mm_mul_ps(v, col1);
        const auto m2 = _mm_mul_ps(v, col2);
        const auto m3 = _mm_mul_ps(v, col3);

        const auto a0 = _mm_hadd_ps(m0, m1);
        const auto a1 = _mm_hadd_ps(m2, m3);
        const auto a3 = _mm_hadd_ps(a0, a1);

        _mm_store_ps(result.v, a3);

        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const Matrix<float, 4, 4>& matrix) noexcept
    {
        const auto v = _mm_load_ps(vector.v);

        const auto col0 = _mm_load_ps(&matrix.m.v[0]);
        const auto col1 = _mm_load_ps(&matrix.m.v[4]);
        const auto col2 = _mm_load_ps(&matrix.m.v[8]);
        const auto col3 = _mm_load_ps(&matrix.m.v[12]);

        const auto m0 = _mm_mul_ps(v, col0);
        const auto m1 = _mm_mul_ps(v, col1);
        const auto m2 = _mm_mul_ps(v, col2);
        const auto m3 = _mm_mul_ps(v, col3);

        const auto a0 = _mm_hadd_ps(m0, m1);
        const auto a1 = _mm_hadd_ps(m2, m3);
        const auto a3 = _mm_hadd_ps(a0, a1);

        _mm_store_ps(vector.v, a3);

        return vector;
    }
#endif
}

#endif // OUZEL_MATH_MATRIX_SSE_HPP
