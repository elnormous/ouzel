// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_QUATERNION_HPP
#define OUZEL_MATH_QUATERNION_HPP

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include "Simd.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T> class Quaternion final
    {
    public:
#if defined(OUZEL_SIMD_SSE) || defined(OUZEL_SIMD_NEON)
        alignas(std::is_same_v<T, float> ? 4 * sizeof(T) : sizeof(T))
#endif
        std::array<T, 4> v;

        [[nodiscard]] auto& operator[](const std::size_t index) noexcept { return v[index]; }
        [[nodiscard]] constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        [[nodiscard]] auto& x() noexcept { return v[0]; }
        [[nodiscard]] constexpr auto x() const noexcept { return v[0]; }

        [[nodiscard]] auto& y() noexcept { return v[1]; }
        [[nodiscard]] constexpr auto y() const noexcept { return v[1]; }

        [[nodiscard]] auto& z() noexcept { return v[2]; }
        [[nodiscard]] constexpr auto z() const noexcept { return v[2]; }

        [[nodiscard]] auto& w() noexcept { return v[3]; }
        [[nodiscard]] constexpr auto w() const noexcept { return v[3]; }

        constexpr void invert() noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; // norm squared
            if (squared <= std::numeric_limits<T>::epsilon()) // too close to zero
                return;

            // conjugate divided by norm squared
            v[0] = -v[0] / squared;
            v[1] = -v[1] / squared;
            v[2] = -v[2] / squared;
            v[3] = v[3] / squared;
        }

        [[nodiscard]] auto getNorm() const noexcept
        {
            constexpr T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == T(1)) // already normalized
                return T(1);

            return std::sqrt(n);
        }

        void getRotation(T& angle, math::Vector<T, 3>& axis) const noexcept
        {
            angle = T(2) * std::acos(v[3]);
            const auto s = std::sqrt(T(1) - v[3] * v[3]);
            if (s <= std::numeric_limits<T>::epsilon()) // too close to zero
            {
                axis.v[0] = v[0];
                axis.v[1] = v[1];
                axis.v[2] = v[2];
            }
            else
            {
                axis.v[0] = v[0] / s;
                axis.v[1] = v[1] / s;
                axis.v[2] = v[2] / s;
            }
        }

        [[nodiscard]] auto& lerp(const Quaternion& q1, const Quaternion& q2, T t) noexcept
        {
            *this = q1 * (T(1) - t) + (q2 * t);
            return *this;
        }
    };

    template <class T>
    constexpr auto identityQuaternion = Quaternion<T>{T(0), T(0), T(0), T(1)};

    template <typename T>
    constexpr void setIdentity(Quaternion<T>& quat) noexcept
    {
        quat.v = {T(0), T(0), T(0), T(1)};
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator==(const Quaternion<T>& quat1,
                                            const Quaternion<T>& quat2) noexcept
    {
        return quat1.v[0] == quat2.v[0] &&
            quat1.v[1] == quat2.v[1] &&
            quat1.v[2] == quat2.v[2] &&
            quat1.v[3] == quat2.v[3];
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator!=(const Quaternion<T>& quat1,
                                            const Quaternion<T>& quat2) noexcept
    {
        return quat1.v[0] != quat2.v[0] ||
            quat1.v[1] != quat2.v[1] ||
            quat1.v[2] != quat2.v[2] ||
            quat1.v[3] != quat2.v[3];
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator+(const Quaternion<T>& quat) noexcept
    {
        return quat;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator-(const Quaternion<T>& quat) noexcept
    {
        return Quaternion<T>{-quat.v[0], -quat.v[1], -quat.v[2], -quat.v[3]};
    }

    template <typename T>
    constexpr void negate(Quaternion<T>& quat) noexcept
    {
        quat.v[0] = -quat.v[0];
        quat.v[1] = -quat.v[1];
        quat.v[2] = -quat.v[2];
        quat.v[3] = -quat.v[3];
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator+(const Quaternion<T>& quat1,
                                           const Quaternion<T>& quat2) noexcept
    {
        return Quaternion<T>{
            quat1.v[0] + quat2.v[0],
            quat1.v[1] + quat2.v[1],
            quat1.v[2] + quat2.v[2],
            quat1.v[3] + quat2.v[3]
        };
    }

    template <typename T>
    constexpr auto& operator+=(Quaternion<T>& quat1,
                               const Quaternion<T>& quat2) noexcept
    {
        quat1.v[0] += quat2.v[0];
        quat1.v[1] += quat2.v[1];
        quat1.v[2] += quat2.v[2];
        quat1.v[3] += quat2.v[3];

        return quat1;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator-(const Quaternion<T>& quat1,
                                           const Quaternion<T>& quat2) noexcept
    {
        return Quaternion<T>{
            quat1.v[0] - quat2.v[0],
            quat1.v[1] - quat2.v[1],
            quat1.v[2] - quat2.v[2],
            quat1.v[3] - quat2.v[3]
        };
    }

    template <typename T>
    constexpr auto& operator-=(Quaternion<T>& quat1,
                               const Quaternion<T>& quat2) noexcept
    {
        quat1.v[0] -= quat2.v[0];
        quat1.v[1] -= quat2.v[1];
        quat1.v[2] -= quat2.v[2];
        quat1.v[3] -= quat2.v[3];

        return quat1;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator*(const Quaternion<T>& quat1,
                                           const Quaternion<T>& quat2) noexcept
    {
        return Quaternion<T>{
            quat1.v[0] * quat2.v[3] + quat1.v[1] * quat2.v[2] - quat1.v[2] * quat2.v[1] + quat1.v[3] * quat2.v[0],
            -quat1.v[0] * quat2.v[2] + quat1.v[1] * quat2.v[3] + quat1.v[2] * quat2.v[0] + quat1.v[3] * quat2.v[1],
            quat1.v[0] * quat2.v[1] - quat1.v[1] * quat2.v[0] + quat1.v[2] * quat2.v[3] + quat1.v[3] * quat2.v[2],
            -quat1.v[0] * quat2.v[0] - quat1.v[1] * quat2.v[1] - quat1.v[2] * quat2.v[2] + quat1.v[3] * quat2.v[3]
        };
    }

    template <typename T>
    constexpr auto& operator*=(Quaternion<T>& quat1,
                               const Quaternion<T>& quat2) noexcept
    {
        quat1.v = {
            quat1.v[0] * quat2.v[3] + quat1.v[1] * quat2.v[2] - quat1.v[2] * quat2.v[1] + quat1.v[3] * quat2.v[0],
            -quat1.v[0] * quat2.v[2] + quat1.v[1] * quat2.v[3] + quat1.v[2] * quat2.v[0] + quat1.v[3] * quat2.v[1],
            quat1.v[0] * quat2.v[1] - quat1.v[1] * quat2.v[0] + quat1.v[2] * quat2.v[3] + quat1.v[3] * quat2.v[2],
            -quat1.v[0] * quat2.v[0] - quat1.v[1] * quat2.v[1] - quat1.v[2] * quat2.v[2] + quat1.v[3] * quat2.v[3]
        };

        return quat1;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator*(const Quaternion<T>& quat,
                                           const T scalar) noexcept
    {
        return Quaternion<T>{
            quat.v[0] * scalar,
            quat.v[1] * scalar,
            quat.v[2] * scalar,
            quat.v[3] * scalar
        };
    }

    template <typename T>
    constexpr auto& operator*=(Quaternion<T>& quat,
                               const T scalar) noexcept
    {
        quat.v[0] *= scalar;
        quat.v[1] *= scalar;
        quat.v[2] *= scalar;
        quat.v[3] *= scalar;

        return quat;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator/(const Quaternion<T>& quat,
                                           const T scalar) noexcept
    {
        return Quaternion<T>{
            quat.v[0] / scalar,
            quat.v[1] / scalar,
            quat.v[2] / scalar,
            quat.v[3] / scalar
        };
    }

    template <typename T>
    constexpr auto& operator/=(Quaternion<T>& quat,
                               const T scalar) noexcept
    {
        quat.v[0] /= scalar;
        quat.v[1] /= scalar;
        quat.v[2] /= scalar;
        quat.v[3] /= scalar;

        return quat;
    }

    template <typename T>
    constexpr void conjugate(Quaternion<T>& quat) noexcept
    {
        quat.v[0] = -quat.v[0];
        quat.v[1] = -quat.v[1];
        quat.v[2] = -quat.v[2];
    }

    template <typename T>
    constexpr auto conjugated(const Quaternion<T>& quat) noexcept
    {
        return Quaternion<T>{-quat.v[0], -quat.v[1], -quat.v[2], quat.v[3]};
    }

    template <typename T>
    void rotate(Quaternion<T>& quat,
                const T angle,
                const math::Vector<T, 3>& axis) noexcept
    {
        const auto normalizedAxis = normalized(axis);

        const auto cosine = std::cos(angle / T(2));
        const auto sine = std::sin(angle / T(2));

        quat.v[0] = normalizedAxis.v[0] * sine;
        quat.v[1] = normalizedAxis.v[1] * sine;
        quat.v[2] = normalizedAxis.v[2] * sine;
        quat.v[3] = cosine;
    }

    template <typename T>
    void rotateVector(math::Vector<T, 3>& vec,
                      const Quaternion<T>& quat) noexcept
    {
        constexpr math::Vector<T, 3> q{quat.v[0], quat.v[1], quat.v[2]};
        const auto t = T(2) * q.cross(vec);
        vec += (quat.v[3] * t) + q.cross(t);
    }

    template <typename T>
    [[nodiscard]] auto rotatedVector(const math::Vector<T, 3>& vec,
                                     const Quaternion<T>& quat) noexcept
    {
        constexpr math::Vector<T, 3> q{quat.v[0], quat.v[1], quat.v[2]};
        const auto t = T(2) * q.cross(vec);
        return vec + (quat.v[3] * t) + q.cross(t);
    }

    template <typename T>
    [[nodiscard]] auto operator*(const Quaternion<T>& quat,
                                 const math::Vector<T, 3>& vec) noexcept
    {
        return rotateVector(vec, quat);
    }

    template <typename T>
    [[nodiscard]] auto getRightVector(const Quaternion<T>& quat) noexcept
    {
        return rotateVector(math::Vector<T, 3>{T(1), T(0), T(0)}, quat);
    }

    template <typename T>
    [[nodiscard]] auto getUpVector(const Quaternion<T>& quat) noexcept
    {
        return rotateVector(math::Vector<T, 3>{T(0), T(1), T(0)}, quat);
    }

    template <typename T>
    [[nodiscard]] auto getForwardVector(const Quaternion<T>& quat) noexcept
    {
        return rotateVector(math::Vector<T, 3>{T(0), T(0), T(1)}, quat);
    }

    template <typename T>
    [[nodiscard]] auto getEulerAngles(const Quaternion<T>& quat) noexcept
    {
        return math::Vector<T, 3>{
            std::atan2(T(2) * (quat.v[1] * quat.v[2] + quat.v[3] * quat.v[0]),
                       quat.v[3] * quat.v[3] - quat.v[0] * quat.v[0] - quat.v[1] * quat.v[1] + quat.v[2] * quat.v[2]),
            std::asin(-T(2) * (quat.v[0] * quat.v[2] - quat.v[3] * quat.v[1])),
            std::atan2(T(2) * (quat.v[0] * quat.v[1] + quat.v[3] * quat.v[2]),
                       quat.v[3] * quat.v[3] + quat.v[0] * quat.v[0] - quat.v[1] * quat.v[1] - quat.v[2] * quat.v[2])
        };
    }

    template <typename T>
    [[nodiscard]] auto getEulerAngleX(const Quaternion<T>& quat) noexcept
    {
        return std::atan2(T(2) * (quat.v[1] * quat.v[2] + quat.v[3] * quat.v[0]),
                          quat.v[3] * quat.v[3] - quat.v[0] * quat.v[0] - quat.v[1] * quat.v[1] + quat.v[2] * quat.v[2]);
    }

    template <typename T>
    [[nodiscard]] auto getEulerAngleY(const Quaternion<T>& quat) noexcept
    {
        return std::asin(T(-2) * (quat.v[0] * quat.v[2] - quat.v[3] * quat.v[1]));
    }

    template <typename T>
    [[nodiscard]] auto getEulerAngleZ(const Quaternion<T>& quat) noexcept
    {
        return std::atan2(T(2) * (quat.v[0] * quat.v[1] + quat.v[3] * quat.v[2]),
                          quat.v[3] * quat.v[3] + quat.v[0] * quat.v[0] - quat.v[1] * quat.v[1] - quat.v[2] * quat.v[2]);
    }

    template <typename T>
    void setEulerAngles(Quaternion<T>& quat,
                        const math::Vector<T, 3>& angles) noexcept
    {
        const auto angleR = angles.v[0] / T(2);
        const auto sr = std::sin(angleR);
        const auto cr = std::cos(angleR);

        const auto angleP = angles.v[1] / T(2);
        const auto sp = std::sin(angleP);
        const auto cp = std::cos(angleP);

        const auto angleY = angles.v[2] / T(2);
        const auto sy = std::sin(angleY);
        const auto cy = std::cos(angleY);

        const auto cpcy = cp * cy;
        const auto spcy = sp * cy;
        const auto cpsy = cp * sy;
        const auto spsy = sp * sy;

        quat.v[0] = sr * cpcy - cr * spsy;
        quat.v[1] = cr * spcy + sr * cpsy;
        quat.v[2] = cr * cpsy - sr * spcy;
        quat.v[3] = cr * cpcy + sr * spsy;
    }
}

#include "QuaternionNeon.hpp"
#include "QuaternionSse.hpp"

#endif // OUZEL_MATH_QUATERNION_HPP
