// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_SCALAR_HPP
#define OUZEL_MATH_SCALAR_HPP

#include "Constants.hpp"

namespace ouzel::math
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
        return x * tau<T> / T(360);
    }

    template <typename T> constexpr auto radToDeg(const T x) noexcept
    {
        return x * T(360) / tau<T>;
    }
}

#endif // OUZEL_MATH_SCALAR_HPP
