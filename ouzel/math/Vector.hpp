// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR_HPP
#define OUZEL_MATH_VECTOR_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace ouzel
{
    template <size_t N, class T> class Vector final
    {
    public:
#if defined(__SSE__)
        alignas(N == 4 ? 4 * sizeof(T) : alignof(T))
#endif
        T v[N]{0};

        constexpr Vector() noexcept = default;

        template <typename ...A>
        constexpr Vector(A... args) noexcept:
            v{static_cast<T>(args)...}
        {
        }

        template <size_t X = N, size_t N2, typename std::enable_if<(X != N2)>::type* = nullptr>
        explicit Vector(const Vector<N2, T>& vec) noexcept
        {
            for (size_t i = 0; i < N && i < N2; ++i)
                v[i] = vec.v[i];
        }

        T& operator[](size_t index) noexcept { return v[index]; }
        constexpr T operator[](size_t index) const noexcept { return v[index]; }

        template <size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        T& x() noexcept { return v[0]; }

        template <size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        constexpr T x() const noexcept { return v[0]; }

        template <size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        T& y() noexcept { return v[1]; }

        template <size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        constexpr T y() const noexcept { return v[1]; }

        template <size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        T& z() noexcept { return v[2]; }

        template <size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        constexpr T z() const noexcept { return v[2]; }

        template <size_t X = N, typename std::enable_if<(X >= 4)>::type* = nullptr>
        T& w() noexcept { return v[3]; }

        template <size_t X = N, typename std::enable_if<(X >= 4)>::type* = nullptr>
        constexpr T w() const noexcept { return v[3]; }

        template <size_t X = N, typename std::enable_if<(X == 2)>::type* = nullptr>
        inline auto getAngle() const
        {
            return atan2(v[1], v[0]);
        }

        template <size_t X = N, typename std::enable_if<(X == 3)>::type* = nullptr>
        inline auto getAngle(const Vector& axis) const
        {
            const T dx = axis.v[0] - v[0] - v[1] * axis.v[2] + v[2] * axis.v[1];
            const T dy = axis.v[1] - v[1] - v[2] * axis.v[0] + v[0] * axis.v[2];
            const T dz = axis.v[2] - v[2] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return atan2(sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        template <size_t X = N, typename std::enable_if<(X == 4)>::type* = nullptr>
        inline auto getAngle(const Vector& axis) const
        {
            const T dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            const T dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            const T dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return atan2(sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector& min, const Vector& max) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] < min.v[i]) v[i] = min.v[i];
                else if (v[i] > max.v[i]) v[i] = max.v[i];
        }

        template <size_t X = N, typename std::enable_if<(X == 3)>::type* = nullptr>
        constexpr Vector cross(const Vector& vec) const noexcept
        {
            return Vector((v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                          (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                          (v[0] * vec.v[1]) - (v[1] * vec.v[0]));
        }

        inline T distance(const Vector& vec) const
        {
            T d = 0;
            for (size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return sqrt(d);
        }

        inline T distanceSquared(const Vector& vec) const noexcept
        {
            T d = 0;
            for (size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return d;
        }

        inline T dot(const Vector& vec) const noexcept
        {
            T d = 0;
            for (size_t i = 0; i < N; ++i)
                d += v[i] * vec.v[i];
            return d;
        }

        inline T length() const
        {
            T l = 0;
            for (const T& c : v)
                l += c * c;
            return sqrt(l);
        }

        inline T lengthSquared() const noexcept
        {
            T l = 0;
            for (const T& c : v)
                l += c * c;
            return l;
        }

        inline void negate() noexcept
        {
            for (T& c : v)
                c = -c;
        }

        inline auto isNormalized(const T tolerance = std::numeric_limits<T>::min()) const
        {
            return abs(T(1) - lengthSquared()) < tolerance;
        }

        void normalize()
        {
            T n = 0;
            for (const T& c : v)
                n += c * c;

            if (n == 1) // already normalized
                return;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return;

            for (T& c : v)
                c /= n;
        }

        Vector normalized() const
        {
            T n = 0;
            for (const T& c : v)
                n += c * c;

            if (n == 1) // already normalized
                return *this;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            n = T(1) / n;
            return *this * n;
        }

        inline void scale(const Vector& scale) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        inline void smooth(const Vector& target, const T elapsedTime, const T responseTime) noexcept
        {
            if (elapsedTime > T(0))
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        inline auto getMin() const noexcept
        {
            T result = v[0];

            for (size_t i = 1; i < N; ++i)
                if (v[i] < result)
                    result = v[i];

            return result;
        }

        inline auto getMax() const noexcept
        {
            T result = v[0];

            for (size_t i = 1; i < N; ++i)
                if (v[i] > result)
                    result = v[i];

            return result;
        }

        inline const Vector operator+(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] += vec.v[i];
            return result;
        }

        inline Vector& operator+=(const Vector& vec) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] += vec.v[i];
            return *this;
        }

        inline const Vector operator-(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] -= vec.v[i];
            return result;
        }

        inline Vector& operator-=(const Vector& vec) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] -= vec.v[i];
            return *this;
        }

        inline const Vector operator-() const noexcept
        {
            Vector result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        inline const Vector operator*(const T scalar) const noexcept
        {
            Vector result(*this);
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        inline Vector& operator*=(const T scalar) noexcept
        {
            for (T& c : v)
                c *= scalar;
            return *this;
        }

        inline const Vector operator/(const T scalar) const noexcept
        {
            Vector result(*this);
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        inline Vector& operator/=(const T scalar) noexcept
        {
            for (T& c : v)
                c /= scalar;
            return *this;
        }

        inline bool operator<(const Vector& vec) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
            {
                if (v[i] < vec.v[i]) return true;
                if (vec.v[i] < v[i]) return false;
            }

            return false;
        }

        inline bool operator==(const Vector& vec) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return false;
            return true;
        }

        inline bool operator!=(const Vector& vec) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return true;
            return false;
        }

        inline auto isZero() const noexcept
        {
            for (const T& c : v)
                if (c != T(0)) return false;
            return true;
        }
    };

    template <size_t N, class T>
    inline const Vector<N, T> operator*(const T scalar, const Vector<N, T>& vec) noexcept
    {
        Vector<N, T> result = vec;
        result *= scalar;
        return result;
    }

    using Vector2F = Vector<2, float>;
    using Vector3F = Vector<3, float>;
    using Vector4F = Vector<4, float>;
}

#endif // OUZEL_MATH_VECTOR_HPP
