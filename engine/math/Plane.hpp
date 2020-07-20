// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_PLANE_HPP
#define OUZEL_MATH_PLANE_HPP

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
        T v[4]{};

        constexpr Plane() noexcept {}

        constexpr Plane(const T a, const T b, const T c, const T d) noexcept:
            v{a, b, c, d}
        {
        }

        T& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return v[index]; }

        T& a() noexcept { return v[0]; }
        constexpr T a() const noexcept { return v[0]; }

        T& b() noexcept { return v[1]; }
        constexpr T b() const noexcept { return v[1]; }

        T& c() noexcept { return v[2]; }
        constexpr T c() const noexcept { return v[2]; }

        T& d() noexcept { return v[3]; }
        constexpr T d() const noexcept { return v[3]; }

        constexpr void flip() noexcept
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        template<std::size_t N, std::enable_if_t<N >= 3>* = nullptr>
        constexpr T dot(const Vector<N, T>& vec) const noexcept
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3];
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

        Plane normalized() const noexcept
        {
            constexpr T squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const T length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const T multiplier = T(1) / length;
            return Plane(v[0] * multiplier,
                         v[1] * multiplier,
                         v[2] * multiplier,
                         v[3] * multiplier);
        }

        constexpr bool operator==(const Plane& plane) const noexcept
        {
            return v[0] == plane.v[0] && v[1] == plane.v[1] && v[2] == plane.v[2] && v[3] == plane.v[3];
        }

        constexpr bool operator!=(const Plane& plane) const noexcept
        {
            return v[0] != plane.v[0] || v[1] != plane.v[1] || v[2] != plane.v[2] || v[3] != plane.v[3];
        }

        static Plane makeFrustumPlane(const T a, const T b, const T c, const T d) noexcept
        {
            const T length = std::sqrt(a * a + b * b + c * c);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return Plane();

            const T multiplier = T(1) / length;
            return Plane(a * multiplier,
                         b * multiplier,
                         c * multiplier,
                         d * multiplier);
        }
    };

    using PlaneF = Plane<float>;
}

#endif // OUZEL_MATH_PLANE_HPP
