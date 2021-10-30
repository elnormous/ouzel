//
// elnormous/omath
//

#ifndef OUZEL_MATH_VECTOR_NEON
#define OUZEL_MATH_VECTOR_NEON

#include "Vector.hpp"

#ifdef __ARM_NEON__
#  include <arm_neon.h>

namespace omath
{
    template <>
    inline auto operator-(const Vector<float, 4>& vector) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v.data(), vnegq_f32(vld1q_f32(vector.v.data())));
        return result;
    }

    template <>
    inline void negate(Vector<float, 4>& vector) noexcept
    {
        vst1q_f32(vector.v.data(), vnegq_f32(vld1q_f32(vector.v.data())));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v.data(), vaddq_f32(vld1q_f32(vector1.v.data()), vld1q_f32(vector2.v.data())));
        return result;
    }

    template <>
    inline auto& operator+=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        vst1q_f32(vector1.v.data(), vaddq_f32(vld1q_f32(vector1.v.data()), vld1q_f32(vector2.v.data())));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Vector<float, 4>& vector1,
                                        const Vector<float, 4>& vector2) noexcept
    {
        Vector<float, 4> result;
        vst1q_f32(result.v.data(), vsubq_f32(vld1q_f32(vector1.v.data()), vld1q_f32(vector2.v.data())));
        return result;
    }

    template <>
    inline auto& operator-=(Vector<float, 4>& vector1,
                            const Vector<float, 4>& vector2) noexcept
    {
        vst1q_f32(vector1.v.data(), vsubq_f32(vld1q_f32(vector1.v.data()), vld1q_f32(vector2.v.data())));
        return vector1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v.data(), vmulq_f32(vld1q_f32(vector.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator*=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(vector.v.data(), vmulq_f32(vld1q_f32(vector.v.data()), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Vector<float, 4>& vector,
                                        const float scalar) noexcept
    {
        Vector<float, 4> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v.data(), vdivq_f32(vld1q_f32(vector.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator/=(Vector<float, 4>& vector,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(vector.v.data(), vdivq_f32(vld1q_f32(vector.v.data()), s));
        return vector;
    }

    template <>
    [[nodiscard]] inline auto dot(const Vector<float, 4>& vector1,
                                  const Vector<float, 4>& vector2) noexcept
    {
        float result;
        const auto t1 = vmulq_f32(vld1q_f32(vector1.v.data()), vld1q_f32(vector2.v.data()));
        const auto t2 = vaddq_f32(t1, vrev64q_f32(t1));
        const auto t3 = vaddq_f32(t2, vcombine_f32(vget_high_f32(t2), vget_low_f32(t2)));
        result = vgetq_lane_f32(t3, 0);
        return result;
    }
}

#endif // __ARM_NEON__

#endif // OUZEL_MATH_VECTOR_NEON
