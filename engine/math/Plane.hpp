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
        std::array<T, 4> v{};

        constexpr Plane() noexcept {}

        constexpr Plane(const T a, const T b, const T c, const T d) noexcept:
            v{{a, b, c, d}}
        {
        }

        auto& operator[](const std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        auto& a() noexcept { return v[0]; }
        constexpr auto a() const noexcept { return v[0]; }

        auto& b() noexcept { return v[1]; }
        constexpr auto b() const noexcept { return v[1]; }

        auto& c() noexcept { return v[2]; }
        constexpr auto c() const noexcept { return v[2]; }

        auto& d() noexcept { return v[3]; }
        constexpr auto d() const noexcept { return v[3]; }

        constexpr auto operator-() const noexcept
        {
            return Plane{-v[0], -v[1], -v[2], -v[3]};
        }

        template<std::size_t N, std::enable_if_t<N >= 3>* = nullptr>
        constexpr auto dot(const Vector<T, N>& vec) const noexcept
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
        }

        void normalize() noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return;

            const auto multiplier = T(1) / length;
            v[0] *= multiplier;
            v[1] *= multiplier;
            v[2] *= multiplier;
            v[3] *= multiplier;
        }

        auto normalized() const noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const auto multiplier = T(1) / length;
            return Plane(v[0] * multiplier,
                         v[1] * multiplier,
                         v[2] * multiplier,
                         v[3] * multiplier);
        }

        constexpr auto operator==(const Plane& plane) const noexcept
        {
            return v[0] == plane.v[0] && v[1] == plane.v[1] && v[2] == plane.v[2] && v[3] == plane.v[3];
        }

        constexpr auto operator!=(const Plane& plane) const noexcept
        {
            return v[0] != plane.v[0] || v[1] != plane.v[1] || v[2] != plane.v[2] || v[3] != plane.v[3];
        }

        static auto makeFrustumPlane(const T a, const T b, const T c, const T d) noexcept
        {
            const auto length = std::sqrt(a * a + b * b + c * c);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return Plane();

            const auto multiplier = T(1) / length;
            return Plane(a * multiplier,
                         b * multiplier,
                         c * multiplier,
                         d * multiplier);
        }
    };
}

#endif // OUZEL_MATH_PLANE_HPP
