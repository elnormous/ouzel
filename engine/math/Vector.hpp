// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR_HPP
#define OUZEL_MATH_VECTOR_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace ouzel
{
    template <typename T, std::size_t N> class Vector final
    {
    public:
#if defined(__SSE__)
        alignas(N == 4 ? 4 * sizeof(T) : alignof(T))
#endif
        std::array<T, N> v{};

        constexpr Vector() noexcept {}

        template <typename ...A>
        explicit constexpr Vector(A... args) noexcept:
            v{static_cast<T>(args)...}
        {
        }

        template <std::size_t X = N, std::size_t N2, std::enable_if_t<(X != N2)>* = nullptr>
        explicit Vector(const Vector<T, N2>& vec) noexcept
        {
            for (std::size_t i = 0; i < N && i < N2; ++i)
                v[i] = vec.v[i];
        }

        auto& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](std::size_t index) const noexcept { return v[index]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        auto& x() noexcept { return v[0]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        constexpr auto x() const noexcept { return v[0]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        auto& y() noexcept { return v[1]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        constexpr auto y() const noexcept { return v[1]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        auto& z() noexcept { return v[2]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        constexpr auto z() const noexcept { return v[2]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 4)>* = nullptr>
        auto& w() noexcept { return v[3]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 4)>* = nullptr>
        constexpr auto w() const noexcept { return v[3]; }

        template <std::size_t X = N, std::enable_if_t<(X == 2)>* = nullptr>
        auto getAngle() const noexcept
        {
            return std::atan2(v[1], v[0]);
        }

        template <std::size_t X = N, std::enable_if_t<(X == 3)>* = nullptr>
        auto getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = axis.v[0] - v[0] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = axis.v[1] - v[1] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = axis.v[2] - v[2] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        template <std::size_t X = N, std::enable_if_t<(X == 4)>* = nullptr>
        auto getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector& min, const Vector& max) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < min.v[i]) v[i] = min.v[i];
                else if (v[i] > max.v[i]) v[i] = max.v[i];
        }

        template <std::size_t X = N, std::enable_if_t<(X == 3)>* = nullptr>
        constexpr Vector cross(const Vector& vec) const noexcept
        {
            return Vector((v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                          (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                          (v[0] * vec.v[1]) - (v[1] * vec.v[0]));
        }

        T distance(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return std::sqrt(d);
        }

        T distanceSquared(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return d;
        }

        T dot(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += v[i] * vec.v[i];
            return d;
        }

        T length() const noexcept
        {
            T l = 0;
            for (const T& c : v)
                l += c * c;
            return std::sqrt(l);
        }

        T lengthSquared() const noexcept
        {
            T l = 0;
            for (const T& c : v)
                l += c * c;
            return l;
        }

        void negate() noexcept
        {
            for (T& c : v)
                c = -c;
        }

        auto isNormalized(const T tolerance = std::numeric_limits<T>::min()) const noexcept
        {
            return std::abs(T(1) - lengthSquared()) < tolerance;
        }

        void normalize() noexcept
        {
            T squared = T(0);
            for (const T& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return;

            const T length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return;

            const T multiplier = T(1) / length;
            for (T& c : v)
                c *= multiplier;
        }

        Vector normalized() const noexcept
        {
            T squared = T(0);
            for (const T& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return *this;

            const T length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const T multiplier = T(1) / length;
            return *this * multiplier;
        }

        void scale(T scalar) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scalar;
        }

        void scale(const Vector& scale) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        template <std::size_t X = N, std::enable_if_t<(X == 2)>* = nullptr>
        void rotate(T angle) noexcept
        {
            const auto sinAngle = std::sin(angle);
            const auto cosAngle = std::cos(angle);

            const auto tempX = v[0] * cosAngle - v[1] * sinAngle;
            v[1] = v[1] * cosAngle + v[0] * sinAngle;
            v[0] = tempX;
        }

        template <std::size_t X = N, std::enable_if_t<(X == 2)>* = nullptr>
        void rotate(const Vector<T, 2>& point, T angle) noexcept
        {
            const float sinAngle = std::sin(angle);
            const float cosAngle = std::cos(angle);

            if (point.isZero())
            {
                const auto tempX = v[0] * cosAngle - v[1] * sinAngle;
                v[1] = v[1] * cosAngle + v[0] * sinAngle;
                v[0] = tempX;
            }
            else
            {
                const auto tempX = v[0] - point.v[0];
                const auto tempY = v[1] - point.v[1];

                v[0] = tempX * cosAngle - tempY * sinAngle + point.v[0];
                v[1] = tempY * cosAngle + tempX * sinAngle + point.v[1];
            }
        }

        void smooth(const Vector& target, const T elapsedTime, const T responseTime) noexcept
        {
            if (elapsedTime > T(0))
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        auto getMin() const noexcept
        {
            T result = v[0];

            for (std::size_t i = 1; i < N; ++i)
                if (v[i] < result)
                    result = v[i];

            return result;
        }

        auto getMax() const noexcept
        {
            T result = v[0];

            for (std::size_t i = 1; i < N; ++i)
                if (v[i] > result)
                    result = v[i];

            return result;
        }

        const Vector operator+(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] += vec.v[i];
            return result;
        }

        Vector& operator+=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] += vec.v[i];
            return *this;
        }

        const Vector operator-(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] -= vec.v[i];
            return result;
        }

        Vector& operator-=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] -= vec.v[i];
            return *this;
        }

        const Vector operator-() const noexcept
        {
            Vector result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        const Vector operator*(const T scalar) const noexcept
        {
            Vector result(*this);
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        Vector& operator*=(const T scalar) noexcept
        {
            for (T& c : v)
                c *= scalar;
            return *this;
        }

        const Vector operator/(const T scalar) const noexcept
        {
            Vector result(*this);
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        Vector& operator/=(const T scalar) noexcept
        {
            for (T& c : v)
                c /= scalar;
            return *this;
        }

        bool operator<(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < vec.v[i]) return true;
                else if (vec.v[i] < v[i]) return false;

            return false;
        }

        bool operator==(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return false;
            return true;
        }

        bool operator!=(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return true;
            return false;
        }

        auto isZero() const noexcept
        {
            for (const T& c : v)
                if (c != T(0)) return false;
            return true;
        }
    };

    template <typename T, std::size_t N>
    const Vector<T, N> operator*(const T scalar, const Vector<T, N>& vec) noexcept
    {
        return vec * scalar;
    }

    using Vector2F = Vector<float, 2>;
    using Vector3F = Vector<float, 3>;
    using Vector4F = Vector<float, 4>;
}

#endif // OUZEL_MATH_VECTOR_HPP
