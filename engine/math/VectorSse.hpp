// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_VECTOR_SSE
#define OUZEL_MATH_VECTOR_SSE

#include "Vector.hpp"

#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
#  include <xmmintrin.h>

namespace ouzel::math
{
    template <>
    inline auto operator-(const Vector<float, 4>& vector) noexcept
    {
        Vector<float, 4> result;
        const auto z = _mm_setzero_ps();
        _mm_store_ps(result.v.data(), _mm_sub_ps(z, _mm_load_ps(vector.v.data())));
        return result;
    }

    template <>
    inline void negate(Vector<float, 4>& vector) noexcept
    {
        const auto z = _mm_setzero_ps();
        _mm_store_ps(vector.v.data(), _mm_sub_ps(z, _mm_load_ps(vector.v.data())));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        _mm_store_ps(result.v.data(), _mm_add_ps(_mm_load_ps(vector1.v.data()), _mm_load_ps(vector2.v.data())));
        return result;
    }

    template <>
    inline auto& operator+=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        _mm_store_ps(vector1.v.data(), _mm_add_ps(_mm_load_ps(vector1.v.data()), _mm_load_ps(vector2.v.data())));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        _mm_store_ps(result.v.data(), _mm_sub_ps(_mm_load_ps(vector1.v.data()), _mm_load_ps(vector2.v.data())));
        return result;
    }

    template <>
    inline auto& operator-=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        _mm_store_ps(vector1.v.data(), _mm_sub_ps(_mm_load_ps(vector1.v.data()), _mm_load_ps(vector2.v.data())));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(result.v.data(), _mm_mul_ps(_mm_load_ps(vector.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(vector.v.data(), _mm_mul_ps(_mm_load_ps(vector.v.data()), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(result.v.data(), _mm_div_ps(_mm_load_ps(vector.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator/=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(vector.v.data(), _mm_div_ps(_mm_load_ps(vector.v.data()), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto dot(const Vector<float, 4>& vector1,
                                  const Vector<float, 4>& vector2) noexcept
    {
        float result;
        const auto t1 = _mm_mul_ps(_mm_load_ps(vector1.v.data()), _mm_load_ps(vector2.v.data()));
        const auto t2 = _mm_add_ps(t1, _mm_shuffle_ps(t1, t1, _MM_SHUFFLE(2, 1, 0, 3)));
        const auto t3 = _mm_add_ps(t2, _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(1, 0, 3, 2)));
        result = _mm_cvtss_f32(t3);
        return result;
    }
}

#endif

#endif // OUZEL_MATH_VECTOR_SSE
