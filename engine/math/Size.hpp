// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_SIZE_HPP
#define OUZEL_MATH_SIZE_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>
#include "Vector.hpp"

namespace ouzel
{
    template <typename T, std::size_t n> class Size final
    {
    public:
        std::array<T, n> v;

        auto& operator[](const std::size_t index) noexcept { return v[index]; }
        constexpr auto operator[](const std::size_t index) const noexcept { return v[index]; }

        template <auto c = n, std::enable_if_t<(c >= 1)>* = nullptr>
        auto& width() noexcept { return v[0]; }

        template <auto c = n, std::enable_if_t<(c >= 1)>* = nullptr>
        constexpr auto width() const noexcept { return v[0]; }

        template <auto c = n, std::enable_if_t<(c >= 2)>* = nullptr>
        auto& height() noexcept { return v[1]; }

        template <auto c = n, std::enable_if_t<(c >= 2)>* = nullptr>
        constexpr auto height() const noexcept { return v[1]; }

        template <auto c = n, std::enable_if_t<(c >= 3)>* = nullptr>
        auto& depth() noexcept { return v[2]; }

        template <auto c = n, std::enable_if_t<(c >= 3)>* = nullptr>
        constexpr auto depth() const noexcept { return v[2]; }

        void scale(const Vector<T, n>& scale) noexcept
        {
            for (std::size_t i = 0; i < n; ++i)
                v[i] *= scale.v[i];
        }

        auto operator+(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < n; ++i)
                result.v[i] += size.v[i];
            return result;
        }

        auto& operator+=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < n; ++i)
                v[i] += size.v[i];
            return *this;
        }

        auto operator-(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < n; ++i)
                result.v[i] -= size.v[i];
            return result;
        }

        auto& operator-=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < n; ++i)
                v[i] -= size.v[i];
            return *this;
        }

        auto operator-() const noexcept
        {
            Size result = *this;
            for (T& c : result.v)
                c = -c;
            return result;
        }

        auto operator*(const T scalar) const noexcept
        {
            Size result = *this;
            for (T& c : result.v)
                c *= scalar;
            return result;
        }

        auto& operator*=(const T scalar) noexcept
        {
            for (auto& c : v)
                c *= scalar;
            return *this;
        }

        auto operator/(const T scalar) const noexcept
        {
            Size result = *this;
            for (T& c : result.v)
                c /= scalar;
            return result;
        }

        auto& operator/=(const T scalar) noexcept
        {
            for (auto& c : v)
                c /= scalar;
            return *this;
        }

        auto operator<(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < n; ++i)
                if (v[i] < size.v[i]) return true;
                else if (size.v[i] < v[i]) return false;

            return false;
        }

        constexpr auto operator==(const Size& size) const noexcept
        {
            return std::equal(std::begin(v), std::end(v), std::begin(size.v));
        }

        constexpr auto operator!=(const Size& size) const noexcept
        {
            return !std::equal(std::begin(v), std::end(v), std::begin(size.v));
        }

        auto volume() const noexcept
        {
            T result = 0;
            for (const auto& c : v)
                result *= c;
            return result;
        }
    };

    template <typename T, std::size_t N>
    auto operator*(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template <typename T, std::size_t N>
    auto operator/(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }
}

#endif // OUZEL_MATH_SIZE_HPP
