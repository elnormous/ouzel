// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_BOX_HPP
#define OUZEL_MATH_BOX_HPP

#include <cstdint>
#include <limits>
#include "Size.hpp"
#include "Vector.hpp"

namespace ouzel
{
    template <std::size_t N, typename T> class Box final
    {
    public:
        Vector<N, T> min;
        Vector<N, T> max;

        Box() noexcept
        {
            for (T& v : min.v)
                v = std::numeric_limits<T>::max();
            for (T& v : max.v)
                v = std::numeric_limits<T>::lowest();
        }

        constexpr Box(const Vector<N, T>& initMin, const Vector<N, T>& initMax) noexcept:
            min(initMin), max(initMax)
        {
        }

        template <std::size_t N2>
        explicit Box(const Box<N2, T>& box) noexcept:
            min(Vector<N, T>(box.min)), max(Vector<N, T>(box.max))
        {
        }

        Vector<N, T> getCenter() const noexcept
        {
            return (min + max) / T(2);
        }

        bool intersects(const Box& aabb) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (aabb.min.v[0] > max.v[0]) return false;
            for (std::size_t i = 0; i < N; ++i)
                if (aabb.max.v[0] < min.v[0]) return false;
            return true;
        }

        bool containsPoint(const Vector<N, T>& point) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (point.v[i] < min.v[i]) return false;
            for (std::size_t i = 0; i < N; ++i)
                if (point.v[i] > max.v[i]) return false;
            return true;
        }

        void merge(const Box& box) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                min.v[i] = std::min(min.v[i], box.min.v[i]);
            for (std::size_t i = 0; i < N; ++i)
                max.v[i] = std::max(max.v[i], box.max.v[i]);
        }

        void reset() noexcept
        {
            for (T& v : min.v)
                v = std::numeric_limits<T>::max();
            for (T& v : max.v)
                v = std::numeric_limits<T>::lowest();
        }

        auto isEmpty() const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (min.v[i] > max.v[i])
                    return true;
            return false;
        }

        void insertPoint(const Vector<N, T>& point) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (point.v[i] < min.v[i]) min.v[i] = point.v[i];
            for (std::size_t i = 0; i < N; ++i)
                if (point.v[i] > max.v[i]) max.v[i] = point.v[i];
        }

        constexpr const Box operator+(const Vector<N, T>& v) const noexcept
        {
            return Box(min + v, max + v);
        }

        Box& operator+=(const Vector<N, T>& v) noexcept
        {
            min += v;
            max += v;
            return *this;
        }

        constexpr const Box operator-(const Vector<N, T>& v) const noexcept
        {
            return Box(min - v, max - v);
        }

        Box& operator-=(const Vector<N, T>& v) noexcept
        {
            min -= v;
            max -= v;
            return *this;
        }

        Size<N, T> getSize() const noexcept
        {
            return Size<N, T>(max - min);
        }
    };

    using Box2F = Box<2, float>;
    using Box3F = Box<3, float>;
}

#endif // OUZEL_MATH_BOX_HPP
