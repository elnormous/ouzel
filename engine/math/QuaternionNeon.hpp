// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_QUATERNION_NEON_HPP
#define OUZEL_MATH_QUATERNION_NEON_HPP

#include "Quaternion.hpp"

#ifdef __ARM_NEON__
#  include <arm_neon.h>

namespace ouzel::math
{
    template <>
    inline auto operator-(const Quaternion<float>& quat) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v, vnegq_f32(vld1q_f32(quat.v)));
        return result;
    }

    template <>
    inline void negate(Quaternion<float>& quat) noexcept
    {
        vst1q_f32(quat.v, vnegq_f32(vld1q_f32(quat.v)));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v, vaddq_f32(vld1q_f32(quat1.v), vld1q_f32(quat2.v)));
        return result;
    }

    template <>
    inline auto& operator+=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        vst1q_f32(quat1.v, vaddq_f32(vld1q_f32(quat1.v), vld1q_f32(quat2.v)));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v, vsubq_f32(vld1q_f32(quat1.v), vld1q_f32(quat2.v)));
        return result;
    }

    template <>
    inline auto& operator-=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        vst1q_f32(quat1.v, vsubq_f32(vld1q_f32(quat1.v), vld1q_f32(quat2.v)));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v, vmulq_f32(vld1q_f32(quat.v), s));
        return result;
    }

    template <>
    inline auto& operator*=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(quat.v, vmulq_f32(vld1q_f32(quat.v), s));
        return quat;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = vdupq_n_f32(1.0F / scalar);
        vst1q_f32(result.v, vmulq_f32(vld1q_f32(quat.v), s));
        return result;
    }

    template <>
    inline auto& operator/=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(1.0F / scalar);
        vst1q_f32(quat.v, vmulq_f32(vld1q_f32(quat.v), s));
        return quat;
    }
}

#endif // __ARM_NEON__

#endif // OUZEL_MATH_QUATERNION_NEON_HPP
