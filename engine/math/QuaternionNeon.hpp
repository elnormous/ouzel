//
// elnormous/omath
//

#ifndef OUZEL_MATH_QUATERNION_NEON
#define OUZEL_MATH_QUATERNION_NEON

#include "Quaternion.hpp"

#ifdef __ARM_NEON__
#  include <arm_neon.h>

namespace omath
{
    template <>
    inline auto operator-(const Quaternion<float>& quat) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v.data(), vnegq_f32(vld1q_f32(quat.v.data())));
        return result;
    }

    template <>
    inline void negate(Quaternion<float>& quat) noexcept
    {
        vst1q_f32(quat.v.data(), vnegq_f32(vld1q_f32(quat.v.data())));
    }

    template <>
    [[nodiscard]] inline auto operator+(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v.data(), vaddq_f32(vld1q_f32(quat1.v.data()), vld1q_f32(quat2.v.data())));
        return result;
    }

    template <>
    inline auto& operator+=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        vst1q_f32(quat1.v.data(), vaddq_f32(vld1q_f32(quat1.v.data()), vld1q_f32(quat2.v.data())));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator-(const Quaternion<float>& quat1,
                                        const Quaternion<float>& quat2) noexcept
    {
        Quaternion<float> result;
        vst1q_f32(result.v.data(), vsubq_f32(vld1q_f32(quat1.v.data()), vld1q_f32(quat2.v.data())));
        return result;
    }

    template <>
    inline auto& operator-=(Quaternion<float>& quat1,
                            const Quaternion<float>& quat2) noexcept
    {
        vst1q_f32(quat1.v.data(), vsubq_f32(vld1q_f32(quat1.v.data()), vld1q_f32(quat2.v.data())));
        return quat1;
    }

    template <>
    [[nodiscard]] inline auto operator*(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v.data(), vmulq_f32(vld1q_f32(quat.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator*=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(quat.v.data(), vmulq_f32(vld1q_f32(quat.v.data()), s));
        return quat;
    }

    template <>
    [[nodiscard]] inline auto operator/(const Quaternion<float>& quat,
                                        const float scalar) noexcept
    {
        Quaternion<float> result;
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(result.v.data(), vdivq_f32(vld1q_f32(quat.v.data()), s));
        return result;
    }

    template <>
    inline auto& operator/=(Quaternion<float>& quat,
                            const float scalar) noexcept
    {
        const auto s = vdupq_n_f32(scalar);
        vst1q_f32(quat.v.data(), vdivq_f32(vld1q_f32(quat.v.data()), s));
        return quat;
    }
}

#endif // __ARM_NEON__

#endif // OUZEL_MATH_QUATERNION_NEON
