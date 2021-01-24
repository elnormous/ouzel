// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
#if defined(__SSE__)
        alignas(4 * sizeof(T))
#endif
        std::array<T, 4> v{};

        constexpr Quaternion() noexcept {}

        constexpr Quaternion(const T x, const T y, const T z, const T w) noexcept:
            v{x, y, z, w}
        {
        }

        auto& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](std::size_t index) const noexcept { return v[index]; }

        auto& x() noexcept { return v[0]; }
        constexpr auto x() const noexcept { return v[0]; }

        auto& y() noexcept { return v[1]; }
        constexpr auto y() const noexcept { return v[1]; }

        auto& z() noexcept { return v[2]; }
        constexpr auto z() const noexcept { return v[2]; }

        auto& w() noexcept { return v[3]; }
        constexpr auto w() const noexcept { return v[3]; }

        static constexpr Quaternion identity() noexcept
        {
            return Quaternion{0, 0, 0, 1};
        }

        constexpr const Quaternion operator*(const Quaternion& q) const noexcept
        {
            return Quaternion{
                v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0],
                -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1],
                v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2],
                -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3]
            };
        }

        constexpr Quaternion& operator*=(const Quaternion& q) noexcept
        {
            const auto tempX = v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0];
            const auto tempY = -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1];
            const auto tempZ = v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2];
            const auto tempW = -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3];

            v[0] = tempX;
            v[1] = tempY;
            v[2] = tempZ;
            v[3] = tempW;

            return *this;
        }

        constexpr const Quaternion operator*(const T scalar) const noexcept
        {
            return Quaternion(v[0] * scalar,
                              v[1] * scalar,
                              v[2] * scalar,
                              v[3] * scalar);
        }

        constexpr Quaternion& operator*=(const T scalar) noexcept
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;

            return *this;
        }

        constexpr const Quaternion operator/(const T scalar) const noexcept
        {
            return Quaternion(v[0] / scalar,
                              v[1] / scalar,
                              v[2] / scalar,
                              v[3] / scalar);
        }

        constexpr Quaternion& operator/=(const T scalar) noexcept
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            v[3] /= scalar;

            return *this;
        }

        constexpr const Quaternion operator-() const noexcept
        {
            return Quaternion(-v[0], -v[1], -v[2], -v[3]);
        }

        constexpr const Quaternion operator+(const Quaternion& q) const noexcept
        {
            return Quaternion(v[0] + q.v[0],
                              v[1] + q.v[1],
                              v[2] + q.v[2],
                              v[3] + q.v[3]);
        }

        constexpr Quaternion& operator+=(const Quaternion& q) noexcept
        {
            v[0] += q.v[0];
            v[1] += q.v[1];
            v[2] += q.v[2];
            v[3] += q.v[3];

            return *this;
        }

        constexpr const Quaternion operator-(const Quaternion& q) const noexcept
        {
            return Quaternion(v[0] - q.v[0],
                              v[1] - q.v[1],
                              v[2] - q.v[2],
                              v[3] - q.v[3]);
        }

        constexpr Quaternion& operator-=(const Quaternion& q) noexcept
        {
            v[0] -= q.v[0];
            v[1] -= q.v[1];
            v[2] -= q.v[2];
            v[3] -= q.v[3];

            return *this;
        }

        constexpr bool operator==(const Quaternion& q) const noexcept
        {
            return v[0] == q.v[0] && v[1] == q.v[1] && v[2] == q.v[2] && v[3] == q.v[3];
        }

        constexpr bool operator!=(const Quaternion& q) const noexcept
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
            constexpr T squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; // norm squared
            if (squared <= std::numeric_limits<T>::min())
                return;

            // conjugate divided by norm squared
            const T multiplier = T(1) / squared;
            v[0] = -v[0] * multiplier;
            v[1] = -v[1] * multiplier;
            v[2] = -v[2] * multiplier;
            v[3] = v[3] * multiplier;
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
            constexpr T squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return;

            const T length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return;

            const T multiplier = T(1) / length;
            v[0] *= multiplier;
            v[1] *= multiplier;
            v[2] *= multiplier;
            v[3] *= multiplier;
        }

        Quaternion normalized() const noexcept
        {
            constexpr T squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const T length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const T multiplier = T(1) / length;
            return Quaternion(v[0] * multiplier,
                              v[1] * multiplier,
                              v[2] * multiplier,
                              v[3] * multiplier);
        }

        void rotate(const T angle, const Vector<T, 3>& axis) noexcept
        {
            const auto normalizedAxis = axis.normalized();

            const T cosAngle = std::cos(angle / T(2));
            const T sinAngle = std::sin(angle / T(2));

            v[0] = normalizedAxis.v[0] * sinAngle;
            v[1] = normalizedAxis.v[1] * sinAngle;
            v[2] = normalizedAxis.v[2] * sinAngle;
            v[3] = cosAngle;
        }

        void getRotation(T& angle, Vector<T, 3>& axis) const noexcept
        {
            angle = T(2) * std::acos(v[3]);
            const T s = std::sqrt(T(1) - v[3] * v[3]);
            if (s <= std::numeric_limits<T>::min()) // too close to zero
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

        Vector<T, 3> getEulerAngles() const noexcept
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
            const T angleR = angles.v[0] / T(2);
            const T sr = std::sin(angleR);
            const T cr = std::cos(angleR);

            const T angleP = angles.v[1] / T(2);
            const T sp = std::sin(angleP);
            const T cp = std::cos(angleP);

            const T angleY = angles.v[2] / T(2);
            const T sy = std::sin(angleY);
            const T cy = std::cos(angleY);

            const T cpcy = cp * cy;
            const T spcy = sp * cy;
            const T cpsy = cp * sy;
            const T spsy = sp * sy;

            v[0] = sr * cpcy - cr * spsy;
            v[1] = cr * spcy + sr * cpsy;
            v[2] = cr * cpsy - sr * spcy;
            v[3] = cr * cpcy + sr * spsy;
        }

        const Vector<T, 3> operator*(const Vector<T, 3>& vector) const noexcept
        {
            return rotateVector(vector);
        }

        Vector<T, 3> rotateVector(const Vector<T, 3>& vector) const noexcept
        {
            constexpr Vector<T, 3> q(v[0], v[1], v[2]);
            const Vector<T, 3> t = T(2) * q.cross(vector);
            return vector + (v[3] * t) + q.cross(t);
        }

        Vector<T, 3> getRightVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(1, 0, 0));
        }

        Vector<T, 3> getUpVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(0, 1, 0));
        }

        Vector<T, 3> getForwardVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(0, 0, 1));
        }

        constexpr Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, T t) noexcept
        {
            *this = (q1 * (T(1) - t)) + (q2 * t);
            return *this;
        }
    };

    using QuaternionF = Quaternion<float>;
}

#endif // OUZEL_MATH_QUATERNION_HPP
