// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE_HPP
#define OUZEL_MATH_SIZE_HPP

#include <cstddef>
#include <type_traits>
#include "math/Vector.hpp"

namespace ouzel
{
    template <size_t N, class T> class Size final
    {
    public:
        T v[N]{0};

        constexpr Size() noexcept {}

        template <typename ...A>
        constexpr Size(A... args) noexcept:
            v{static_cast<T>(args)...}
        {
        }

        template <size_t X = N, size_t N2, typename std::enable_if<(X != N2)>::type* = nullptr>
        explicit Size(const Size<N2, T>& size) noexcept
        {
            for (size_t i = 0; i < N && i < N2; ++i)
                v[i] = size.v[i];
        }

        explicit Size(const Vector<N, T>& vec) noexcept:
            v(vec.v)
        {
        }

        inline T& operator[](size_t index) noexcept { return v[index]; }
        constexpr T operator[](size_t index) const noexcept { return v[index]; }

        template <size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        inline T& width() noexcept { return v[0]; }

        template <size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        constexpr T width() const noexcept { return v[0]; }

        template <size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        inline T& height() noexcept { return v[1]; }

        template <size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        constexpr T height() const noexcept { return v[1]; }

        template <size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        inline T& depth() noexcept { return v[2]; }

        template <size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        constexpr T depth() const noexcept { return v[2]; }

        inline void scale(const Vector<N, T>& scale) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        inline const Size operator+(const Size& size) const noexcept
        {
            Size result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] += size.v[i];
            return result;
        }

        inline Size& operator+=(const Size& size) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] += size.v[i];
            return *this;
        }

        inline const Size operator-(const Size& size) const noexcept
        {
            Size result = *this;
            for (size_t i = 0; i < N; ++i)
                result.v[i] -= size.v[i];
            return result;
        }

        inline Size& operator-=(const Size& size) noexcept
        {
            for (size_t i = 0; i < N; ++i)
                v[i] -= size.v[i];
            return *this;
        }

        inline const Size operator-() const noexcept
        {
            Size result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        inline const Size operator*(const T scalar) const noexcept
        {
            Size result(*this);
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        inline Size& operator*=(const T scalar) noexcept
        {
            for (T& c : v)
                c *= scalar;
            return *this;
        }

        inline const Size operator/(const T scalar) const noexcept
        {
            Size result(*this);
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        inline Size& operator/=(const T scalar) noexcept
        {
            for (T& c : v)
                c /= scalar;
            return *this;
        }

        inline bool operator<(const Size& size) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
            {
                if (v[i] < size.v[i]) return true;
                else if (size.v[i] < v[i]) return false;
            }

            return false;
        }

        inline bool operator==(const Size& size) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return false;
            return true;
        }

        inline bool operator!=(const Size& size) const noexcept
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return true;
            return false;
        }

        inline auto isZero() const noexcept
        {
            for (const T& c : v)
                if (c != T(0)) return false;
            return true;
        }

        inline T volume() const noexcept
        {
            T result = 0;
            for (const T& c : v)
                result *= c;

            return result;
        }
    };

    template <size_t N, class T>
    inline const Size<N, T> operator*(const Size<N, T>& size, const Vector<N, T>& v) noexcept
    {
        Size<N, T> result = size;
        for (size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template <size_t N, class T>
    inline const Size<N, T> operator/(const Size<N, T>& size, const Vector<N, T>& v) noexcept
    {
        Size<N, T> result = size;
        for (size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }

    using Size2U = Size<2, uint32_t>;
    using Size3U = Size<3, uint32_t>;
    using Size2F = Size<2, float>;
    using Size3F = Size<3, float>;
}

#endif // OUZEL_MATH_SIZE_HPP
