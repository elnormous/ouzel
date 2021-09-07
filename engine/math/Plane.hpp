// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_PLANE_HPP
#define OUZEL_MATH_PLANE_HPP

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include "Vector.hpp"
#include "MathUtils.hpp"

namespace ouzel
{
    template <typename T> class Plane final
    {
    public:
#ifdef __SSE__
        alignas(4 * sizeof(T))
#endif
        std::array<T, 4> v;

        [[nodiscard]] auto& operator[](const std::size_t index) noexcept { return v[index]; }
        [[nodiscard]] constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        [[nodiscard]] auto& a() noexcept { return v[0]; }
        [[nodiscard]] constexpr auto a() const noexcept { return v[0]; }

        [[nodiscard]] auto& b() noexcept { return v[1]; }
        [[nodiscard]] constexpr auto b() const noexcept { return v[1]; }

        [[nodiscard]] auto& c() noexcept { return v[2]; }
        [[nodiscard]] constexpr auto c() const noexcept { return v[2]; }

        [[nodiscard]] auto& d() noexcept { return v[3]; }
        [[nodiscard]] constexpr auto d() const noexcept { return v[3]; }

        [[nodiscard]] auto operator+() const noexcept
        {
            return *this;
        }
        
        [[nodiscard]] constexpr auto operator-() const noexcept
        {
            return Plane{-v[0], -v[1], -v[2], -v[3]};
        }

        [[nodiscard]] constexpr auto dot(const Vector<T, 3>& vec) const noexcept
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
        }

        [[nodiscard]] auto distance(const Vector<T, 3>& vec) const
        {
            return std::abs(dot(vec)) /
                std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
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

        [[nodiscard]] auto normalized() const noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return *this;

            return Plane{
                v[0] / length,
                v[1] / length,
                v[2] / length,
                v[3] / length
            };
        }

        [[nodiscard]] constexpr auto operator==(const Plane& plane) const noexcept
        {
            return v[0] == plane.v[0] && v[1] == plane.v[1] && v[2] == plane.v[2] && v[3] == plane.v[3];
        }

        [[nodiscard]] constexpr auto operator!=(const Plane& plane) const noexcept
        {
            return v[0] != plane.v[0] || v[1] != plane.v[1] || v[2] != plane.v[2] || v[3] != plane.v[3];
        }

        [[nodiscard]] static auto makeFrustumPlane(const T a, const T b, const T c, const T d) noexcept
        {
            const auto length = std::sqrt(a * a + b * b + c * c);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return Plane{};

            return Plane{
                a / length,
                b / length,
                c / length,
                d / length
            };
        }
    };
}

#endif // OUZEL_MATH_PLANE_HPP
