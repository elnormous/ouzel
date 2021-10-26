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

        [[nodiscard]] auto intersects(const Box& aabb) const noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
            {
                if (aabb.min.v[0] > max.v[0]) return false;
                if (aabb.max.v[0] < min.v[0]) return false;
            }

            return true;
        }

        [[nodiscard]] auto containsPoint(const math::Vector<T, dims>& point) const noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
            {
                if (point.v[i] < min.v[i]) return false;
                if (point.v[i] > max.v[i]) return false;
            }

            return true;
        }

        void merge(const Box& box) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
            {
                min.v[i] = std::min(min.v[i], box.min.v[i]);
                max.v[i] = std::max(max.v[i], box.max.v[i]);
            }
        }

        void reset() noexcept
        {
            for (T& v : min.v)
                v = std::numeric_limits<T>::max();
            for (T& v : max.v)
                v = std::numeric_limits<T>::lowest();
        }

        [[nodiscard]] auto isEmpty() const noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
                if (min.v[i] > max.v[i])
                    return true;
            return false;
        }

        void insertPoint(const math::Vector<T, dims>& point) noexcept
        {
            for (std::size_t i = 0; i < dims; ++i)
            {
                if (point.v[i] < min.v[i]) min.v[i] = point.v[i];
                if (point.v[i] > max.v[i]) max.v[i] = point.v[i];
            }
        }

        [[nodiscard]] constexpr auto operator+(const math::Vector<T, dims>& v) const noexcept
        {
            return Box{min + v, max + v};
        }

        auto& operator+=(const math::Vector<T, dims>& v) noexcept
        {
            min += v;
            max += v;
            return *this;
        }

        [[nodiscard]] constexpr auto operator-(const math::Vector<T, dims>& v) const noexcept
        {
            return Box{min - v, max - v};
        }

        auto& operator-=(const math::Vector<T, dims>& v) noexcept
        {
            min -= v;
            max -= v;
            return *this;
        }

        [[nodiscard]] auto getSize() const noexcept
        {
            return math::Size<T, dims>{max - min};
        }
    };
}

#endif // OUZEL_MATH_BOX_HPP
