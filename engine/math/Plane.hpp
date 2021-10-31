// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_PLANE_HPP
#define OUZEL_MATH_PLANE_HPP

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include "Scalar.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T> class Plane final
    {
    public:
#if defined(__SSE__) || defined(_M_X64) || _M_IX86_FP >= 1 || defined(__ARM_NEON__)
        alignas(std::is_same_v<T, float> ? 4 * sizeof(T) : sizeof(T))
#endif
#if (defined(__SSE2__) || defined(_M_X64) || _M_IX86_FP >= 2) || (defined(__ARM_NEON__) && defined(__aarch64__))
        alignas(std::is_same_v<T, double> ? 4 * sizeof(T) : sizeof(T))
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
    };

    template <typename T>
    [[nodiscard]] constexpr auto operator+(const Plane<T>& plane) noexcept
    {
        return plane;
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator-(const Plane<T>& plane) noexcept
    {
        return Plane{-plane.v[0], -plane.v[1], -plane.v[2], -plane.v[3]};
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator==(const Plane<T>& plane1,
                                            const Plane<T>& plane2) noexcept
    {
        return plane1.v[0] == plane2.v[0] && plane1.v[1] == plane2.v[1] && plane1.v[2] == plane2.v[2] && plane1.v[3] == plane2.v[3];
    }

    template <typename T>
    [[nodiscard]] constexpr auto operator!=(const Plane<T>& plane1,
                                            const Plane<T>& plane2) noexcept
    {
        return plane1.v[0] != plane2.v[0] || plane1.v[1] != plane2.v[1] || plane1.v[2] != plane2.v[2] || plane1.v[3] != plane2.v[3];
    }

    template <typename T>
    [[nodiscard]] constexpr auto dot(const Plane<T>& plane,
                                     const math::Vector<T, 3>& vec) noexcept
    {
        return plane.v[0] * vec.v[0] + plane.v[1] * vec.v[1] + plane.v[2] * vec.v[2] + plane.v[3];
    }

    template <typename T>
    [[nodiscard]] auto distance(const Plane<T>& plane,
                                const math::Vector<T, 3>& vec) noexcept(false)
    {
        return std::abs(dot(vec)) /
            std::sqrt(plane.v[0] * plane.v[0] + plane.v[1] * plane.v[1] + plane.v[2] * plane.v[2]);
    }

    template <typename T>
    void normalize(Plane<T>& plane) noexcept
    {
        constexpr auto squared = plane.v[0] * plane.v[0] + plane.v[1] * plane.v[1] + plane.v[2] * plane.v[2] + plane.v[3] * plane.v[3];
        if (squared == T(1)) // already normalized
            return;

        const auto length = std::sqrt(squared);
        if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
            return;

        plane.v[0] /= length;
        plane.v[1] /= length;
        plane.v[2] /= length;
        plane.v[3] /= length;
    }

    template <typename T>
    [[nodiscard]] auto normalized(const Plane<T>& plane) noexcept
    {
        constexpr auto squared = plane.v[0] * plane.v[0] + plane.v[1] * plane.v[1] + plane.v[2] * plane.v[2] + plane.v[3] * plane.v[3];
        if (squared == T(1)) // already normalized
            return plane;

        const auto length = std::sqrt(squared);
        if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
            return plane;

        return Plane<T>{
            plane.v[0] / length,
            plane.v[1] / length,
            plane.v[2] / length,
            plane.v[3] / length
        };
    }

    template <typename T>
    [[nodiscard]] auto makeFrustumPlane(const T a, const T b, const T c, const T d) noexcept
    {
        const auto length = std::sqrt(a * a + b * b + c * c);
        if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
            return Plane<T>{};

        return Plane<T>{
            a / length,
            b / length,
            c / length,
            d / length
        };
    }
}

#endif // OUZEL_MATH_PLANE_HPP
