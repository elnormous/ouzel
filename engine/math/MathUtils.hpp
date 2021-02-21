// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATHUTILS_HPP
#define OUZEL_MATH_MATHUTILS_HPP

#include <cstdint>
#include <cmath>
#include <limits>
#include <type_traits>

namespace ouzel
{
    template <typename T>
    constexpr auto lerp(const T v0, const T v1, const T t) noexcept
    {
        return (T(1) - t) * v0 + t * v1;
    }

    template <typename T>
    constexpr auto smoothStep(const T a, const T b, const T t) noexcept
    {
        return lerp(a, b, t * t * (T(3) - T(2) * t));
    }

    template <typename T> constexpr auto degToRad(const T x) noexcept
    {
        return x * T(0.01745329251994329576);
    }

    template <typename T> constexpr auto radToDeg(const T x) noexcept
    {
        return x * T(57.2957795130823208767);
    }

    template <typename T>
    constexpr auto isNearlyEqual(const T a, const T b,
                                 const T tolerance = std::numeric_limits<T>::min()) noexcept
    {
        return (a - b) <= tolerance && (a - b) >= -tolerance;
    }
}

#endif // OUZEL_MATH_MATHUTILS_HPP
