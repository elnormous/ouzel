// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_VECTOR_HPP
#define OUZEL_MATH_VECTOR_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

namespace ouzel
{
    template <typename T, std::size_t dims> class Vector final
    {
    public:
#ifdef __SSE__
        alignas(dims == 4 ? 4 * sizeof(T) : alignof(T))
#endif
        std::array<T, dims> v;

        constexpr Vector() noexcept = default;

        template <typename ...A>
        explicit constexpr Vector(const A... args) noexcept:
            v{args...}
        {
        }

        template <auto c = dims, std::size_t d, std::enable_if_t<(c != d)>* = nullptr>
        explicit Vector(const Vector<T, d>& vec) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                v[i] = (i < d) ? vec.v[i] : T(0);
        }

        [[nodiscard]] auto& operator[](const std::size_t index) noexcept { return v[index]; }
        [[nodiscard]] constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        [[nodiscard]] auto& x() noexcept
        {
            static_assert(dims >= 1);
            return v[0];
        }

        [[nodiscard]] constexpr auto x() const noexcept
        {
            static_assert(dims >= 1);
            return v[0];
        }

        [[nodiscard]] auto& y() noexcept
        {
            static_assert(dims >= 2);
            return v[1];
        }

        [[nodiscard]] constexpr auto y() const noexcept
        {
            static_assert(dims >= 2);
            return v[1];
        }

        [[nodiscard]] auto& z() noexcept
        {
            static_assert(dims >= 3);
            return v[2];
        }

        [[nodiscard]] constexpr auto z() const noexcept
        {
            static_assert(dims >= 3);
            return v[2];
        }

        [[nodiscard]] auto& w() noexcept
        {
            static_assert(dims >= 4);
            return v[3];
        }

        [[nodiscard]] constexpr auto w() const noexcept
        {
            static_assert(dims >= 4);
            return v[3];
        }

        template <auto c = dims, std::enable_if_t<(c == 2)>* = nullptr>
        [[nodiscard]] auto getAngle() const noexcept
        {
            return std::atan2(v[1], v[0]);
        }

        template <auto c = dims, std::enable_if_t<(c == 3)>* = nullptr>
        [[nodiscard]] auto getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = axis.v[0] - v[0] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = axis.v[1] - v[1] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = axis.v[2] - v[2] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        template <auto c = dims, std::enable_if_t<(c == 4)>* = nullptr>
        [[nodiscard]] auto getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector& min, const Vector& max) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                if (v[i] < min.v[i]) v[i] = min.v[i];
                else if (v[i] > max.v[i]) v[i] = max.v[i];
        }

        [[nodiscard]] constexpr auto cross(const Vector& vec) const noexcept
        {
            static_assert(dims == 3);

            return Vector{
                (v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                (v[0] * vec.v[1]) - (v[1] * vec.v[0])
            };
        }

        [[nodiscard]] auto distance(const Vector& vec) const noexcept
        {
            return std::sqrt(generateDistanceSquared(std::make_index_sequence<dims>{}, vec));
        }

        [[nodiscard]] constexpr auto distanceSquared(const Vector& vec) const noexcept
        {
            return generateDistanceSquared(std::make_index_sequence<dims>{}, vec);
        }

        [[nodiscard]] auto length() const noexcept
        {
            return std::sqrt(generateLengthSquared(std::make_index_sequence<dims>{}));
        }

        [[nodiscard]] constexpr auto lengthSquared() const noexcept
        {
            return generateLengthSquared(std::make_index_sequence<dims>{});
        }

        [[nodiscard]] constexpr auto dot(const Vector& vec) const noexcept
        {
            return generateDot(std::make_index_sequence<dims>{}, vec);
        }

        void negate() noexcept
        {
            for (auto& c : v)
                c = -c;
        }

        [[nodiscard]] auto isNormalized(const T tolerance = std::numeric_limits<T>::epsilon()) const noexcept
        {
            return std::abs(T(1) - lengthSquared()) <= tolerance;
        }

        void normalize() noexcept
        {
            const auto squared = lengthSquared();
            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return;

            for (auto& c : v)
                c /= length;
        }

        [[nodiscard]] auto normalized() const noexcept
        {
            const auto squared = lengthSquared();
            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return *this;

            return *this / length;
        }

        template <auto c = dims, std::enable_if_t<(c == 2)>* = nullptr>
        void rotate(const T angle) noexcept
        {
            const auto sine = std::sin(angle);
            const auto cosine = std::cos(angle);

            const auto tempX = v[0] * cosine - v[1] * sine;
            v[1] = v[1] * cosine + v[0] * sine;
            v[0] = tempX;
        }

        template <auto c = dims, std::enable_if_t<(c == 2)>* = nullptr>
        void rotate(const Vector<T, 2>& point, const T angle) noexcept
        {
            const auto sine = std::sin(angle);
            const auto cosine = std::cos(angle);

            if (point.v[0] == T(0) || point.v[1] == T(0))
            {
                const auto tempX = v[0] * cosine - v[1] * sine;
                v[1] = v[1] * cosine + v[0] * sine;
                v[0] = tempX;
            }
            else
            {
                const auto tempX = v[0] - point.v[0];
                const auto tempY = v[1] - point.v[1];

                v[0] = tempX * cosine - tempY * sine + point.v[0];
                v[1] = tempY * cosine + tempX * sine + point.v[1];
            }
        }

        void smooth(const Vector& target, const T elapsedTime, const T responseTime) noexcept
        {
            if (elapsedTime > T(0))
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        [[nodiscard]] constexpr auto operator+() const noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr auto operator-() const noexcept
        {
            return generateInverse(std::make_index_sequence<dims>{});
        }

        [[nodiscard]] constexpr auto operator+(const Vector& vec) const noexcept
        {
            return generateSum(std::make_index_sequence<dims>{}, vec);
        }

        auto& operator+=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                v[i] += vec.v[i];
            return *this;
        }

        [[nodiscard]] constexpr auto operator-(const Vector& vec) const noexcept
        {
            return generateDiff(std::make_index_sequence<dims>{}, vec);
        }

        auto& operator-=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                v[i] -= vec.v[i];
            return *this;
        }

        [[nodiscard]] constexpr auto operator*(const T scalar) const noexcept
        {
            return generateMul(std::make_index_sequence<dims>{}, scalar);
        }

        auto& operator*=(const T scalar) noexcept
        {
            for (auto& c : v)
                c *= scalar;
            return *this;
        }

        [[nodiscard]] constexpr auto operator/(const T scalar) const noexcept
        {
            return generateDiv(std::make_index_sequence<dims>{}, scalar);
        }

        auto& operator/=(const T scalar) noexcept
        {
            for (auto& c : v)
                c /= scalar;
            return *this;
        }

        [[nodiscard]] constexpr auto operator<(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                if (v[i] < vec.v[i]) return true;
                else if (vec.v[i] < v[i]) return false;

            return false;
        }

        [[nodiscard]] constexpr auto operator==(const Vector& vec) const noexcept
        {
            return std::equal(std::begin(v), std::end(v), std::begin(vec.v));
        }

        [[nodiscard]] constexpr auto operator!=(const Vector& vec) const noexcept
        {
            return !std::equal(std::begin(v), std::end(v), std::begin(vec.v));
        }

    private:
        template <std::size_t ...I>
        constexpr auto generateInverse(const std::index_sequence<I...>) const noexcept
        {
            return Vector{-v[I]...};
        }

        template <std::size_t ...I>
        constexpr auto generateSum(const std::index_sequence<I...>, const Vector& vec) const noexcept
        {
            return Vector{(v[I] + vec.v[I])...};
        }

        template <std::size_t ...I>
        constexpr auto generateDiff(const std::index_sequence<I...>, const Vector& vec) const noexcept
        {
            return Vector{(v[I] - vec.v[I])...};
        }

        template <std::size_t ...I>
        constexpr auto generateMul(const std::index_sequence<I...>, const T scalar) const noexcept
        {
            return Vector{(v[I] * scalar)...};
        }

        template <std::size_t ...I>
        constexpr auto generateDiv(const std::index_sequence<I...>, const T scalar) const noexcept
        {
            return Vector{(v[I] / scalar)...};
        }

        template <std::size_t ...I>
        constexpr auto generateLengthSquared(const std::index_sequence<I...>) const noexcept
        {
            return ((v[I] * v[I]) + ...);
        }

        template <std::size_t ...I>
        constexpr auto generateDot(const std::index_sequence<I...>, const Vector& vec) const noexcept
        {
            return ((v[I] * vec.v[I]) + ...);
        }

        template <std::size_t ...I>
        constexpr auto generateDistanceSquared(const std::index_sequence<I...>, const Vector& vec) const noexcept
        {
            return (((v[I] - vec.v[I]) * (v[I] - vec.v[I])) + ...);
        }
    };

    template <typename T, std::size_t N>
    [[nodiscard]] auto operator*(const T scalar, const Vector<T, N>& vec) noexcept
    {
        return vec * scalar;
    }
}

#endif // OUZEL_MATH_VECTOR_HPP
