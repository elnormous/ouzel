// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_MATHUTILS_HPP
#define OUZEL_MATH_MATHUTILS_HPP

#include <cstdint>
#include <cmath>
#include <limits>
#include <type_traits>
#if defined(__ANDROID__)
#  include <cpu-features.h>
#endif

namespace ouzel
{
#if defined(__ARM_NEON__)
#  if defined(__ANDROID__) && defined(__arm__)
    // NEON support must be checked at runtime on 32-bit Android
    class AnrdoidNeonChecker final
    {
    public:
        AnrdoidNeonChecker() noexcept:
            neonAvailable(android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
                          (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
        {
        }

        operator bool() const noexcept { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNeonChecker isSimdAvailable;
#  else
    constexpr auto isSimdAvailable = true;
#  endif
#elif defined(__SSE__)
    constexpr auto isSimdAvailable = true;
#else
    constexpr auto isSimdAvailable = false;
#endif

    template <typename T> constexpr T lerp(const T v0, const T v1, const T t) noexcept
    {
        return (T(1) - t) * v0 + t * v1;
    }

    template <typename T> constexpr T smoothStep(const T a, const T b, const T t) noexcept
    {
        return lerp(a, b, t * t * (T(3) - T(2) * t));
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    constexpr auto isPowerOfTwo(const T x) noexcept
    {
        return (x != T(0)) && (((x - T(1)) & x) == 0);
    }

    template <typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T nextPowerOfTwo(const T x) noexcept
    {
        if (x != 0)
        {
            --x;
            for (uint32_t shift = 1; shift < sizeof(T) * 8; shift *= 2)
                x |= (x >> shift);
        }
        return ++x;
    }

    template <typename T> constexpr T degToRad(const T x) noexcept
    {
        return static_cast<T>(x * 0.01745329251994329576);
    }

    template <typename T> constexpr T radToDeg(const T x) noexcept
    {
        return static_cast<T>(x * 57.2957795130823208767);
    }

    template <typename T> constexpr T clamp(const T x, const T lo, const T hi) noexcept
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
    }

    template <typename T>
    constexpr auto isNearlyEqual(const T a, const T b, const T tolerance = std::numeric_limits<T>::min()) noexcept
    {
        return (a - b) <= tolerance && (a - b) >= -tolerance;
    }
}

#endif // OUZEL_MATH_MATHUTILS_HPP
