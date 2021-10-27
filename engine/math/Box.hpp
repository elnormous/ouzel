// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_BOX_HPP
#define OUZEL_MATH_BOX_HPP

#include <cstdint>
#include <limits>
#include "Size.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T, std::size_t dims> class Box final
    {
    public:
        math::Vector<T, dims> min;
        math::Vector<T, dims> max;

        Box() noexcept
        {
            for (T& v : min.v)
                v = std::numeric_limits<T>::max();
            for (T& v : max.v)
                v = std::numeric_limits<T>::lowest();
        }

        constexpr Box(const math::Vector<T, dims>& initMin, const math::Vector<T, dims>& initMax) noexcept:
            min{initMin}, max{initMax}
        {
        }

        template <std::size_t dims2>
        explicit Box(const math::Box<T, dims2>& box) noexcept:
            min{Vector<T, dims>{box.min}}, max{Vector<T, dims>{box.max}}
        {
        }

        [[nodiscard]] auto getCenter() const noexcept
        {
            return (min + max) / T(2);
        }
    };

    template <typename T, std::size_t dims>
    [[nodiscard]] constexpr auto operator+(const Box<T, dims>& box,
                                           const math::Vector<T, dims>& v) noexcept
    {
        return Box<T, dims>{box.min + v, box.max + v};
    }

    template <typename T, std::size_t dims>
    auto& operator+=(Box<T, dims>& box,
                     const math::Vector<T, dims>& v) noexcept
    {
        box.min += v;
        box.max += v;
        return box;
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] constexpr auto operator-(const Box<T, dims>& box,
                                           const math::Vector<T, dims>& v) noexcept
    {
        return Box<T, dims>{box.min - v, box.max - v};
    }

    template <typename T, std::size_t dims>
    auto& operator-=(Box<T, dims>& box,
                     const math::Vector<T, dims>& v) noexcept
    {
        box.min -= v;
        box.max -= v;
        return box;
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] auto isEmpty(const Box<T, dims>& box) noexcept
    {
        for (std::size_t i = 0; i < dims; ++i)
            if (box.min.v[i] > box.max.v[i])
                return true;

        return false;
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] auto getSize(const Box<T, dims>& box) noexcept
    {
        return math::Size<T, dims>{box.max - box.min};
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] auto containsPoint(const Box<T, dims>& box,
                                     const math::Vector<T, dims>& point) noexcept
    {
        for (std::size_t i = 0; i < dims; ++i)
            if (point.v[i] < box.min.v[i] || point.v[i] > box.max.v[i])
                return false;

        return true;
    }

    template <typename T, std::size_t dims>
    void merge(Box<T, dims>& box1,
               const Box<T, dims>& box2) noexcept
    {
        for (std::size_t i = 0; i < dims; ++i)
        {
            box1.min.v[i] = std::min(box1.min.v[i], box2.min.v[i]);
            box1.max.v[i] = std::max(box1.max.v[i], box2.max.v[i]);
        }
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] auto merged(const Box<T, dims>& box1,
                              const Box<T, dims>& box2) noexcept
    {
        Box<T, dims> result;

        for (std::size_t i = 0; i < dims; ++i)
        {
            result.min.v[i] = std::min(box1.min.v[i], box2.min.v[i]);
            result.max.v[i] = std::max(box1.max.v[i], box2.max.v[i]);
        }

        return result;
    }

    template <typename T, std::size_t dims>
    [[nodiscard]] auto intersects(const Box<T, dims>& box1,
                                  const Box<T, dims>& box2) noexcept
    {
        for (std::size_t i = 0; i < dims; ++i)
            if (box2.min.v[i] > box1.max.v[i] || box2.max.v[i] < box1.min.v[i])
                return false;

        return true;
    }

    template <typename T, std::size_t dims>
    void reset(Box<T, dims>& box) noexcept
    {
        for (T& v : box.min.v)
            v = std::numeric_limits<T>::max();
        for (T& v : box.max.v)
            v = std::numeric_limits<T>::lowest();
    }

    template <typename T, std::size_t dims>
    void insertPoint(Box<T, dims>& box,
                     const math::Vector<T, dims>& point) noexcept
    {
        for (std::size_t i = 0; i < dims; ++i)
        {
            if (point.v[i] < box.min.v[i]) box.min.v[i] = point.v[i];
            if (point.v[i] > box.max.v[i]) box.max.v[i] = point.v[i];
        }
    }
}

#endif // OUZEL_MATH_BOX_HPP
