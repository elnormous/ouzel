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
        AnrdoidNeonChecker()
        {
            neonAvailable = (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
                             (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0);
        }

        operator bool() const { return neonAvailable; }

    private:
        bool neonAvailable;
    };

    extern AnrdoidNeonChecker isSimdAvailable;
#  else
    constexpr bool isSimdAvailable = true;
#  endif
#elif defined(__SSE__)
    constexpr bool isSimdAvailable = true;
#else
    constexpr bool isSimdAvailable = false;
#endif

    constexpr double TAU = 6.28318530717958647692;
    constexpr double PI = 3.14159265358979323846;
    constexpr double SQRT2 = 1.4142135623730950488;

    template<typename T> inline T lerp(T v0, T v1, T t)
    {
        return (1 - t) * v0 + t * v1;
    }

    template<typename T> inline T smoothStep(T a, T b, T t)
    {
        float remapSmoothStep = t * t * (3 - 2 * t);
        return lerp(a, b, remapSmoothStep);
    }

    template<typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline bool isPowerOfTwo(T x)
    {
        return (x != 0) && (((x - 1) & x) == 0);
    }

    template<typename T, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
    inline T nextPowerOfTwo(T x)
    {
        if (x != 0)
        {
            --x;
            for (uint32_t shift = 1; shift < sizeof(T) * 8; shift *= 2)
                x |= (x >> shift);
        }
        return ++x;
    }

    template<typename T> inline int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T> inline T degToRad(T x)
    {
        return static_cast<T>(x * 0.01745329252);
    }

    template<typename T> inline T radToDeg(T x)
    {
        return static_cast<T>(x * 57.295779513);
    }

    template<typename T>
    inline T clamp(T x, T lo, T hi)
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
    }

    constexpr uint64_t INITIAL_FNV = 2166136261U;
    constexpr uint64_t FNV_MULTIPLE = 16777619;

    // Fowler / Noll / Vo (FNV) hash
    inline uint64_t fnvHash(uint64_t s)
    {
        uint64_t hash = INITIAL_FNV;
        for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
        {
            hash = hash ^ (reinterpret_cast<uint8_t*>(&s)[i]); // xor the low 8 bits
            hash = hash * FNV_MULTIPLE; // multiply by the magic number
        }
        return hash;
    }

    template<class T>
    inline bool isNearlyEqual(T a, T b, T tolerance = std::numeric_limits<T>::min())
    {
        return fabs(a - b) <= tolerance;
    }
}

#endif // OUZEL_MATH_MATHUTILS_HPP
