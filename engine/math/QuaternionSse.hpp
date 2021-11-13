// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_QUATERNION_SSE_HPP
#define OUZEL_MATH_QUATERNION_SSE_HPP

#include "Quaternion.hpp"

#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
#  include <xmmintrin.h>
#endif

#if defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2
#  include <emmintrin.h>
#endif

#ifdef __SSE3__
#  include <pmmintrin.h>
#endif // __SSE3__

namespace ouzel::math
{
#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1
    template <>
    inline auto operator-(const Quaternion<float>& quat) noexcept
    {
        Quaternion<float> result;
        const auto z = _mm_setzero_ps();
        _mm_store_ps(result.v, _mm_sub_ps(z, _mm_load_ps(quat.v)));
        return result;
    }

    template <>
    inline void negate(Quaternion<float>& quat) noexcept
    {
        const auto z = _mm_setzero_ps();
        _mm_store_ps(quat.v, _mm_sub_ps(z, _mm_load_ps(quat.v)));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        _mm_store_ps(result.v, _mm_add_ps(_mm_load_ps(quat1.v), _mm_load_ps(quat2.v)));
        return result;
    }

    template <>
    inline auto& operator+=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        _mm_store_ps(quat1.v, _mm_add_ps(_mm_load_ps(quat1.v), _mm_load_ps(quat2.v)));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        _mm_store_ps(result.v, _mm_sub_ps(_mm_load_ps(quat1.v), _mm_load_ps(quat2.v)));
        return result;
    }

    template <>
    inline auto& operator-=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        _mm_store_ps(quat1.v, _mm_sub_ps(_mm_load_ps(quat1.v), _mm_load_ps(quat2.v)));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(result.v, _mm_mul_ps(_mm_load_ps(quat.v), s));
        return result;
    }

    template <>
    inline auto& operator*=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(quat.v, _mm_mul_ps(_mm_load_ps(quat.v), s));
        return quat;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(result.v, _mm_div_ps(_mm_load_ps(quat.v), s));
        return result;
    }

    template <>
    inline auto& operator/=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = _mm_set1_ps(scalar);
        _mm_store_ps(quat.v, _mm_div_ps(_mm_load_ps(quat.v), s));
        return quat;
    }
#endif // SSE

#if defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2
#  ifndef __AVX__
    template <>
    inline auto operator-(const Quaternion<double>& quat) noexcept
    {
        Quaternion<double> result;
        const auto z = _mm_setzero_pd();
        _mm_store_pd(&result.v[0], _mm_sub_pd(z, _mm_load_pd(&quat.v[0])));
        _mm_store_pd(&result.v[2], _mm_sub_pd(z, _mm_load_pd(&quat.v[2])));
        return result;
    }

    template <>
    inline void negate(Quaternion<double>& quat) noexcept
    {
        const auto z = _mm_setzero_pd();
        _mm_store_pd(&quat.v[0], _mm_sub_pd(z, _mm_load_pd(&quat.v[0])));
        _mm_store_pd(&quat.v[2], _mm_sub_pd(z, _mm_load_pd(&quat.v[2])));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Quaternion<double>& quat1,
                                        const Quaternion<double>& quat2) noexcept
    {
        Quaternion<double> result;
        _mm_store_pd(&result.v[0], _mm_add_pd(_mm_load_pd(&quat1.v[0]), _mm_load_pd(&quat2.v[0])));
        _mm_store_pd(&result.v[2], _mm_add_pd(_mm_load_pd(&quat1.v[2]), _mm_load_pd(&quat2.v[2])));
        return result;
    }

    template <>
    inline auto& operator+=(Quaternion<double>& quat1,
                            const Quaternion<double>& quat2) noexcept
    {
        _mm_store_pd(&quat1.v[0], _mm_add_pd(_mm_load_pd(&quat1.v[0]), _mm_load_pd(&quat2.v[0])));
        _mm_store_pd(&quat1.v[2], _mm_add_pd(_mm_load_pd(&quat1.v[2]), _mm_load_pd(&quat2.v[2])));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Quaternion<double>& quat1,
                                        const Quaternion<double>& quat2) noexcept
    {
        Quaternion<double> result;
        _mm_store_pd(&result.v[0], _mm_sub_pd(_mm_load_pd(&quat1.v[0]), _mm_load_pd(&quat2.v[0])));
        _mm_store_pd(&result.v[2], _mm_sub_pd(_mm_load_pd(&quat1.v[2]), _mm_load_pd(&quat2.v[2])));
        return result;
    }

    template <>
    inline auto& operator-=(Quaternion<double>& quat1,
                            const Quaternion<double>& quat2) noexcept
    {
        _mm_store_pd(&quat1.v[0], _mm_sub_pd(_mm_load_pd(&quat1.v[0]), _mm_load_pd(&quat2.v[0])));
        _mm_store_pd(&quat1.v[2], _mm_sub_pd(_mm_load_pd(&quat1.v[2]), _mm_load_pd(&quat2.v[2])));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Quaternion<double>& quat,
                                        const double scalar) noexcept
    {
        Quaternion<double> result;
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&result.v[0], _mm_mul_pd(_mm_load_pd(&quat.v[0]), s));
        _mm_store_pd(&result.v[2], _mm_mul_pd(_mm_load_pd(&quat.v[2]), s));
        return result;
    }

    template <>
    inline auto& operator*=(Quaternion<double>& quat,
                            const double scalar) noexcept
    {
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&quat.v[0], _mm_mul_pd(_mm_load_pd(&quat.v[0]), s));
        _mm_store_pd(&quat.v[2], _mm_mul_pd(_mm_load_pd(&quat.v[2]), s));
        return quat;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Quaternion<double>& quat,
                                        const double scalar) noexcept
    {
        Quaternion<double> result;
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&result.v[0], _mm_div_pd(_mm_load_pd(&quat.v[0]), s));
        _mm_store_pd(&result.v[2], _mm_div_pd(_mm_load_pd(&quat.v[2]), s));
        return result;
    }

    template <>
    inline auto& operator/=(Quaternion<double>& quat,
                            const double scalar) noexcept
    {
        const auto s = _mm_set1_pd(scalar);
        _mm_store_pd(&quat.v[0], _mm_div_pd(_mm_load_pd(&quat.v[0]), s));
        _mm_store_pd(&quat.v[2], _mm_div_pd(_mm_load_pd(&quat.v[2]), s));
        return quat;
    }
#  endif // __AVX__
#endif // SSE2

#ifdef __SSE3__
    template <>
    [[nodiscard]] inline auto norm(const Quaternion<float>& quat) noexcept
    {
        const auto v = _mm_load_ps(quat.v);
        const auto t1 = _mm_mul_ps(v, v);
        const auto t2 = _mm_hadd_ps(t1, t1);
        const auto t3 = _mm_hadd_ps(t2, t2);
        const auto s = _mm_sqrt_ps(t3);
        return _mm_cvtss_f32(s);
    }
#endif // __SSE3__
}

#endif // OUZEL_MATH_QUATERNION_SSE_HPP
