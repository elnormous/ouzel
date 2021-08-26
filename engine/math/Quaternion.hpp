// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_QUATERNION_HPP
#define OUZEL_MATH_QUATERNION_HPP

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include "Vector.hpp"

namespace ouzel
{
    template <typename T> class Quaternion final
    {
    public:
#ifdef __SSE__
        alignas(4 * sizeof(T))
#endif
        std::array<T, 4> v;

        auto& operator[](const std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        [[nodiscard]] auto& x() noexcept { return v[0]; }
        [[nodiscard]] constexpr auto x() const noexcept { return v[0]; }

        [[nodiscard]] auto& y() noexcept { return v[1]; }
        [[nodiscard]] constexpr auto y() const noexcept { return v[1]; }

        [[nodiscard]] auto& z() noexcept { return v[2]; }
        [[nodiscard]] constexpr auto z() const noexcept { return v[2]; }

        [[nodiscard]] auto& w() noexcept { return v[3]; }
        [[nodiscard]] constexpr auto w() const noexcept { return v[3]; }

        [[nodiscard]] static constexpr auto identity() noexcept
        {
            return Quaternion{T(0), T(0), T(0), T(1)};
        }

        [[nodiscard]] constexpr auto operator*(const Quaternion& q) const noexcept
        {
            return Quaternion{
                v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0],
                -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1],
                v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2],
                -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3]
            };
        }

        constexpr auto& operator*=(const Quaternion& q) noexcept
        {
            v = {
                v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0],
                -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1],
                v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2],
                -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3]
            };

            return *this;
        }

        constexpr auto operator*(const T scalar) const noexcept
        {
            return Quaternion{
                v[0] * scalar,
                v[1] * scalar,
                v[2] * scalar,
                v[3] * scalar
            };
        }

        constexpr auto& operator*=(const T scalar) noexcept
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;

            return *this;
        }

        constexpr auto operator/(const T scalar) const noexcept
        {
            return Quaternion{
                v[0] / scalar,
                v[1] / scalar,
                v[2] / scalar,
                v[3] / scalar
            };
        }

        constexpr auto& operator/=(const T scalar) noexcept
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            v[3] /= scalar;

            return *this;
        }

        constexpr auto operator-() const noexcept
        {
            return Quaternion{-v[0], -v[1], -v[2], -v[3]};
        }

        constexpr auto operator+(const Quaternion& q) const noexcept
        {
            return Quaternion{
                v[0] + q.v[0],
                v[1] + q.v[1],
                v[2] + q.v[2],
                v[3] + q.v[3]
            };
        }

        constexpr auto& operator+=(const Quaternion& q) noexcept
        {
            v[0] += q.v[0];
            v[1] += q.v[1];
            v[2] += q.v[2];
            v[3] += q.v[3];

            return *this;
        }

        constexpr auto operator-(const Quaternion& q) const noexcept
        {
            return Quaternion{
                v[0] - q.v[0],
                v[1] - q.v[1],
                v[2] - q.v[2],
                v[3] - q.v[3]
            };
        }

        constexpr auto& operator-=(const Quaternion& q) noexcept
        {
            v[0] -= q.v[0];
            v[1] -= q.v[1];
            v[2] -= q.v[2];
            v[3] -= q.v[3];

            return *this;
        }

        constexpr auto operator==(const Quaternion& q) const noexcept
        {
            return v[0] == q.v[0] && v[1] == q.v[1] && v[2] == q.v[2] && v[3] == q.v[3];
        }

        constexpr auto operator!=(const Quaternion& q) const noexcept
        {
            return v[0] != q.v[0] || v[1] != q.v[1] || v[2] != q.v[2] || v[3] != q.v[3];
        }

        constexpr void negate() noexcept
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        constexpr void conjugate() noexcept
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

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

        auto getNorm() const noexcept
        {
            constexpr T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == T(1)) // already normalized
                return T(1);

            return std::sqrt(n);
        }

        void normalize() noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return;

            v[0] /= length;
            v[1] /= length;
            v[2] /= length;
            v[3] /= length;
        }

        auto normalized() const noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return *this;

            return Quaternion{
                v[0] / length,
                v[1] / length,
                v[2] / length,
                v[3] / length
            };
        }

        void rotate(const T angle, const Vector<T, 3>& axis) noexcept
        {
            const auto normalizedAxis = axis.normalized();

            const auto cosine = std::cos(angle / T(2));
            const auto sine = std::sin(angle / T(2));

            v[0] = normalizedAxis.v[0] * sine;
            v[1] = normalizedAxis.v[1] * sine;
            v[2] = normalizedAxis.v[2] * sine;
            v[3] = cosine;
        }

        void getRotation(T& angle, Vector<T, 3>& axis) const noexcept
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

        auto getEulerAngles() const noexcept
        {
            return Vector<T, 3>{
                std::atan2(2 * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]),
                std::asin(-2 * (v[0] * v[2] - v[3] * v[1])),
                std::atan2(2 * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2])
            };
        }

        auto getEulerAngleX() const noexcept
        {
            return std::atan2(T(2) * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
        }

        auto getEulerAngleY() const noexcept
        {
            return std::asin(T(-2) * (v[0] * v[2] - v[3] * v[1]));
        }

        auto getEulerAngleZ() const noexcept
        {
            return std::atan2(T(2) * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
        }

        void setEulerAngles(const Vector<T, 3>& angles) noexcept
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

            v[0] = sr * cpcy - cr * spsy;
            v[1] = cr * spcy + sr * cpsy;
            v[2] = cr * cpsy - sr * spcy;
            v[3] = cr * cpcy + sr * spsy;
        }

        auto operator*(const Vector<T, 3>& vector) const noexcept
        {
            return rotateVector(vector);
        }

        auto rotateVector(const Vector<T, 3>& vector) const noexcept
        {
            constexpr Vector<T, 3> q{v[0], v[1], v[2]};
            const auto t = T(2) * q.cross(vector);
            return vector + (v[3] * t) + q.cross(t);
        }

        auto getRightVector() const noexcept
        {
            return rotateVector(Vector<T, 3>{T(1), T(0), T(0)});
        }

        auto getUpVector() const noexcept
        {
            return rotateVector(Vector<T, 3>{T(0), T(1), T(0)});
        }

        auto getForwardVector() const noexcept
        {
            return rotateVector(Vector<T, 3>{T(0), T(0), T(1)});
        }

        constexpr auto& lerp(const Quaternion& q1, const Quaternion& q2, T t) noexcept
        {
            *this = q1 * (T(1) - t) + (q2 * t);
            return *this;
        }
    };
}

#endif // OUZEL_MATH_QUATERNION_HPP
