// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_VECTOR_HPP
#define OUZEL_MATH_VECTOR_HPP

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

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
        explicit constexpr Vector(const A... args) noexcept:
            v{args...}
        {
        }

        template <auto X = N, std::size_t M, std::enable_if_t<(X != M)>* = nullptr>
        explicit Vector(const Vector<T, M>& vec) noexcept
        {
            for (std::size_t i = 0; i < N && i < M; ++i)
                v[i] = vec.v[i];
        }

        auto& operator[](const std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        template <auto X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        auto& x() noexcept { return v[0]; }

        template <auto X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        constexpr auto x() const noexcept { return v[0]; }

        template <auto X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        auto& y() noexcept { return v[1]; }

        template <auto X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        constexpr auto y() const noexcept { return v[1]; }

        template <auto X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        auto& z() noexcept { return v[2]; }

        template <auto X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        constexpr auto z() const noexcept { return v[2]; }

        template <auto X = N, std::enable_if_t<(X >= 4)>* = nullptr>
        auto& w() noexcept { return v[3]; }

        template <auto X = N, std::enable_if_t<(X >= 4)>* = nullptr>
        constexpr auto w() const noexcept { return v[3]; }

        template <auto X = N, std::enable_if_t<(X == 2)>* = nullptr>
        auto getAngle() const noexcept
        {
            return std::atan2(v[1], v[0]);
        }

        template <auto X = N, std::enable_if_t<(X == 3)>* = nullptr>
        auto getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = axis.v[0] - v[0] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = axis.v[1] - v[1] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = axis.v[2] - v[2] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        template <auto X = N, std::enable_if_t<(X == 4)>* = nullptr>
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

        template <auto X = N, std::enable_if_t<(X == 3)>* = nullptr>
        constexpr auto cross(const Vector& vec) const noexcept
        {
            return Vector{
                (v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                (v[0] * vec.v[1]) - (v[1] * vec.v[0])
            };
        }

        auto distance(const Vector& vec) const noexcept
        {
            T d = T(0);
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return std::sqrt(d);
        }

        auto distanceSquared(const Vector& vec) const noexcept
        {
            T d = T(0);
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return d;
        }

        constexpr auto dot(const Vector& vec) const noexcept
        {
            return generateDot(std::make_index_sequence<N>{}, vec);
        }

        auto length() const noexcept
        {
            T l = T(0);
            for (const auto& c : v)
                l += c * c;
            return std::sqrt(l);
        }

        auto lengthSquared() const noexcept
        {
            T l = T(0);
            for (const auto& c : v)
                l += c * c;
            return l;
        }

        void negate() noexcept
        {
            for (auto& c : v)
                c = -c;
        }

        auto isNormalized(const T tolerance = std::numeric_limits<T>::min()) const noexcept
        {
            return std::abs(T(1) - lengthSquared()) < tolerance;
        }

        void normalize() noexcept
        {
            T squared = T(0);
            for (const auto& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return;

            const auto multiplier = T(1) / length;
            for (auto& c : v)
                c *= multiplier;
        }

        auto normalized() const noexcept
        {
            T squared = T(0);
            for (const auto& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const auto multiplier = T(1) / length;
            return *this * multiplier;
        }

        void scale(const T scalar) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scalar;
        }

        void scale(const Vector& scale) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        template <auto X = N, std::enable_if_t<(X == 2)>* = nullptr>
        void rotate(const T angle) noexcept
        {
            const auto sinAngle = std::sin(angle);
            const auto cosAngle = std::cos(angle);

            const auto tempX = v[0] * cosAngle - v[1] * sinAngle;
            v[1] = v[1] * cosAngle + v[0] * sinAngle;
            v[0] = tempX;
        }

        template <auto X = N, std::enable_if_t<(X == 2)>* = nullptr>
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

        constexpr const auto operator-() const noexcept
        {
            return generateInverse(std::make_index_sequence<N>{});
        }

        constexpr const auto operator+(const Vector& vec) const noexcept
        {
            return generateSum(std::make_index_sequence<N>{}, vec);
        }

        auto& operator+=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] += vec.v[i];
            return *this;
        }

        constexpr const auto operator-(const Vector& vec) const noexcept
        {
            return generateDiff(std::make_index_sequence<N>{}, vec);
        }

        auto& operator-=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] -= vec.v[i];
            return *this;
        }

        constexpr const auto operator*(const T scalar) const noexcept
        {
            return generateMul(std::make_index_sequence<N>{}, scalar);
        }

        auto& operator*=(const T scalar) noexcept
        {
            for (auto& c : v)
                c *= scalar;
            return *this;
        }

        constexpr const auto operator/(const T scalar) const noexcept
        {
            return generateDiv(std::make_index_sequence<N>{}, scalar);
        }

        auto& operator/=(const T scalar) noexcept
        {
            for (auto& c : v)
                c /= scalar;
            return *this;
        }

        auto operator<(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < vec.v[i]) return true;
                else if (vec.v[i] < v[i]) return false;

            return false;
        }

        auto operator==(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return false;
            return true;
        }

        auto operator!=(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return true;
            return false;
        }

        auto isZero() const noexcept
        {
            for (const auto& c : v)
                if (c != T(0)) return false;
            return true;
        }

    private:
        template <std::size_t...I>
        constexpr auto generateInverse(const std::index_sequence<I...>) const
        {
            return Vector{-v[I]...};
        }

        template <std::size_t...I>
        constexpr auto generateSum(const std::index_sequence<I...>, const Vector& vec) const
        {
            return Vector{(v[I] + vec.v[I])...};
        }

        template <std::size_t...I>
        constexpr auto generateDiff(const std::index_sequence<I...>, const Vector& vec) const
        {
            return Vector{(v[I] - vec.v[I])...};
        }

        template <std::size_t...I>
        constexpr auto generateMul(const std::index_sequence<I...>, T scalar) const
        {
            return Vector{(v[I] * scalar)...};
        }

        template <std::size_t...I>
        constexpr auto generateDiv(const std::index_sequence<I...>, T scalar) const
        {
            return Vector{(v[I] / scalar)...};
        }

        template<typename... Args>
        static constexpr auto sum(Args... args) noexcept
        {
            return (args + ...);
        }

        template <std::size_t...I>
        constexpr auto generateDot(const std::index_sequence<I...>, const Vector& vec) const
        {
            return sum((v[I] * vec.v[I])...);
        }
    };

    template <typename T, std::size_t N>
    const auto operator*(const T scalar, const Vector<T, N>& vec) noexcept
    {
        return vec * scalar;
    }
}

#endif // OUZEL_MATH_VECTOR_HPP
