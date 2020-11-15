// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE_HPP
#define OUZEL_MATH_SIZE_HPP

#include <cstddef>
#include <type_traits>
#include "Vector.hpp"

namespace ouzel
{
    template <std::size_t N, typename T> class Size final
    {
    public:
        T v[N]{};

        constexpr Size() noexcept {}

        template <typename ...A>
        explicit constexpr Size(A... args) noexcept:
            v{static_cast<T>(args)...}
        {
        }

        template <std::size_t X = N, std::size_t N2, std::enable_if_t<(X != N2)>* = nullptr>
        explicit Size(const Size<N2, T>& size) noexcept
        {
            for (std::size_t i = 0; i < N && i < N2; ++i)
                v[i] = size.v[i];
        }

        explicit constexpr Size(const Vector<N, T>& vec) noexcept:
            v{vec.v}
        {
        }

        T& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return v[index]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        T& width() noexcept { return v[0]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        constexpr T width() const noexcept { return v[0]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        T& height() noexcept { return v[1]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        constexpr T height() const noexcept { return v[1]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        T& depth() noexcept { return v[2]; }

        template <std::size_t X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        constexpr T depth() const noexcept { return v[2]; }

        void scale(const Vector<N, T>& scale) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        const Size operator+(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] += size.v[i];
            return result;
        }

        Size& operator+=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] += size.v[i];
            return *this;
        }

        const Size operator-(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] -= size.v[i];
            return result;
        }

        Size& operator-=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] -= size.v[i];
            return *this;
        }

        const Size operator-() const noexcept
        {
            Size result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        const Size operator*(const T scalar) const noexcept
        {
            Size result(*this);
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        Size& operator*=(const T scalar) noexcept
        {
            for (T& c : v)
                c *= scalar;
            return *this;
        }

        const Size operator/(const T scalar) const noexcept
        {
            Size result(*this);
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        Size& operator/=(const T scalar) noexcept
        {
            for (T& c : v)
                c /= scalar;
            return *this;
        }

        bool operator<(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
            {
                if (v[i] < size.v[i]) return true;
                else if (size.v[i] < v[i]) return false;
            }

            return false;
        }

        bool operator==(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return false;
            return true;
        }

        bool operator!=(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return true;
            return false;
        }

        auto isZero() const noexcept
        {
            for (const T& c : v)
                if (c != T(0)) return false;
            return true;
        }

        T volume() const noexcept
        {
            T result = 0;
            for (const T& c : v)
                result *= c;
            return result;
        }
    };

    template <std::size_t N, typename T>
    const Size<N, T> operator*(const Size<N, T>& size, const Vector<N, T>& v) noexcept
    {
        Size<N, T> result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template <std::size_t N, typename T>
    const Size<N, T> operator/(const Size<N, T>& size, const Vector<N, T>& v) noexcept
    {
        Size<N, T> result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }

    using Size2U = Size<2, std::uint32_t>;
    using Size3U = Size<3, std::uint32_t>;
    using Size2F = Size<2, float>;
    using Size3F = Size<3, float>;
}

#endif // OUZEL_MATH_SIZE_HPP
