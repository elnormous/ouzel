// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_VECTOR_NEON_HPP
#define OUZEL_MATH_VECTOR_NEON_HPP

#include "Vector.hpp"

#ifdef __ARM_NEON__
#  include <arm_neon.h>

namespace ouzel::math
{
    template <>
    inline auto operator-(const Vector<float, 4>& vector) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v, vnegq_f32(vld1q_f32(vector.v)));
        return result;
    }

    template <>
    inline void negate(Vector<float, 4>& vector) noexcept
    {
        vst1q_f32(vector.v, vnegq_f32(vld1q_f32(vector.v)));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v, vaddq_f32(vld1q_f32(vector1.v), vld1q_f32(vector2.v)));
        return result;
    }

    template <>
    inline auto& operator+=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        vst1q_f32(vector1.v, vaddq_f32(vld1q_f32(vector1.v), vld1q_f32(vector2.v)));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v, vsubq_f32(vld1q_f32(vector1.v), vld1q_f32(vector2.v)));
        return result;
    }

    template <>
    inline auto& operator-=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        vst1q_f32(vector1.v, vsubq_f32(vld1q_f32(vector1.v), vld1q_f32(vector2.v)));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v, vmulq_f32(vld1q_f32(vector.v), s));
        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(vector.v, vmulq_f32(vld1q_f32(vector.v), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v, vdivq_f32(vld1q_f32(vector.v), s));
        return result;
    }

    template <>
    inline auto& operator/=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(vector.v, vdivq_f32(vld1q_f32(vector.v), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto dot(const Vector<float, 4>& vector1,
                                  const Vector<float, 4>& vector2) noexcept
    {
        const auto t1 = vmulq_f32(vld1q_f32(vector1.v), vld1q_f32(vector2.v));
        const auto t2 = vaddq_f32(t1, vrev64q_f32(t1));
        const auto t3 = vaddq_f32(t2, vcombine_f32(vget_high_f32(t2), vget_low_f32(t2)));
        const float result = vgetq_lane_f32(t3, 0);
        return result;
    }
}

#endif // __ARM_NEON__

#endif // OUZEL_MATH_VECTOR_NEON_HPP
