// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_SIZE_HPP
#define OUZEL_MATH_SIZE_HPP

#include <array>
#include <cstddef>
#include <type_traits>
#include "Vector.hpp"

namespace ouzel
{
    template <typename T, std::size_t N> class Size final
    {
    public:
        std::array<T, N> v{};

        constexpr Size() noexcept {}

        template <typename ...A>
        explicit constexpr Size(const A... args) noexcept:
            v{args...}
        {
        }

        template <auto X = N, std::size_t M, std::enable_if_t<(X != M)>* = nullptr>
        explicit Size(const Size<T, M>& size) noexcept
        {
            for (std::size_t i = 0; i < N && i < M; ++i)
                v[i] = size.v[i];
        }

        explicit constexpr Size(const Vector<T, N>& vec) noexcept:
            v{vec.v}
        {
        }

        auto& operator[](const std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        template <auto X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        auto& width() noexcept { return v[0]; }

        template <auto X = N, std::enable_if_t<(X >= 1)>* = nullptr>
        constexpr auto width() const noexcept { return v[0]; }

        template <auto X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        auto& height() noexcept { return v[1]; }

        template <auto X = N, std::enable_if_t<(X >= 2)>* = nullptr>
        constexpr auto height() const noexcept { return v[1]; }

        template <auto X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        auto& depth() noexcept { return v[2]; }

        template <auto X = N, std::enable_if_t<(X >= 3)>* = nullptr>
        constexpr auto depth() const noexcept { return v[2]; }

        void scale(const Vector<T, N>& scale) noexcept
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
            Size result = *this;
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
            Size result = *this;
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
                if (v[i] < size.v[i]) return true;
                else if (size.v[i] < v[i]) return false;

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

    template <typename T, std::size_t N>
    const Size<T, N> operator*(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template <typename T, std::size_t N>
    const Size<T, N> operator/(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }
}

#endif // OUZEL_MATH_SIZE_HPP
