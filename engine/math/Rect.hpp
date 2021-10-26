// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_RECT_HPP
#define OUZEL_MATH_RECT_HPP

#include "Size.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T> class Rect final
    {
    public:
        math::Vector<T, 2> position;
        math::Size<T, 2> size;

        constexpr Rect() noexcept {}

        constexpr Rect(const T width, const T height) noexcept:
            size{width, height}
        {
        }

        constexpr Rect(const T x, const T y,
                       const T width, const T height) noexcept:
            position{x, y}, size{width, height}
        {
        }

        constexpr Rect(const math::Vector<T, 2>& initPosition,
                       const math::Size<T, 2>& initSize) noexcept:
            position{initPosition}, size{initSize}
        {
        }

        constexpr void setPosition(const T x, const T y) noexcept
        {
            position.v[0] = x;
            position.v[1] = y;
        }

        constexpr void setPosition(const math::Vector<T, 2>& newPosition) noexcept
        {
            position = newPosition;
        }

        [[nodiscard]] constexpr auto left() const noexcept
        {
            return position.v[0];
        }

        [[nodiscard]] constexpr auto bottom() const noexcept
        {
            return position.v[1];
        }

        [[nodiscard]] constexpr auto right() const noexcept
        {
            return position.v[0] + size.v[0];
        }

        [[nodiscard]] constexpr auto top() const noexcept
        {
            return position.v[1] + size.v[1];
        }

        [[nodiscard]] constexpr auto bottomLeft() const noexcept
        {
            return position;
        }

        [[nodiscard]] constexpr auto topRight() const noexcept
        {
            return math::Vector<T, 2>{position.v[0] + size.v[0], position.v[1] + size.v[1]};
        }

        [[nodiscard]] static auto intersect(const Rect& r1, const Rect& r2, Rect& dst) noexcept
        {
            const T xmin = std::max(r1.position.v[0], r2.position.v[0]);
            const T xmax = std::min(r1.right(), r2.right());
            if (xmax > xmin)
            {
                const T ymin = std::max(r1.position.v[1], r2.position.v[1]);
                const T ymax = std::min(r1.bottom(), r2.bottom());
                if (ymax > ymin)
                {
                    dst.position.v[0] = xmin;
                    dst.position.v[1] = ymin;
                    dst.size.v[0] = xmax - xmin;
                    dst.size.v[1] = ymax - ymin;
                    return true;
                }
            }

            dst.position.v[0] = dst.position.v[1] = dst.size.v[0] = dst.size.v[1] = 0;
            return false;
        }

        static void combine(const Rect& r1, const Rect& r2, Rect& dst) noexcept
        {
            dst.position.v[0] = std::min(r1.position.v[0], r2.position.v[0]);
            dst.position.v[1] = std::min(r1.position.v[1], r2.position.v[1]);
            dst.size.v[0] = std::max(r1.position.v[0] + r1.size.v[0], r2.position.v[0] + r2.size.v[0]) - dst.position.v[0];
            dst.size.v[1] = std::max(r1.position.v[1] + r1.size.v[1], r2.position.v[1] + r2.size.v[1]) - dst.position.v[1];
        }
    };

    template <class T>
    [[nodiscard]] constexpr auto operator==(const Rect<T>& rect1,
                                            const Rect<T>& rect2) noexcept
    {
        return rect1.position == rect2.position && rect1.size == rect2.size;
    }

    template <class T>
    [[nodiscard]] constexpr auto operator!=(const Rect<T>& rect1,
                                            const Rect<T>& rect2) noexcept
    {
        return rect1.position != rect2.position || rect1.size != rect2.size;
    }

    template <class T>
    [[nodiscard]] constexpr auto operator*(const Rect<T>& rect,
                                           const T scalar) noexcept
    {
        return Rect{
            rect.position * scalar,
            rect.size * scalar
        };
    }

    template <class T>
    constexpr auto& operator*=(Rect<T>& rect,
                               const T scalar) noexcept
    {
        rect.position *= scalar;
        rect.size *= scalar;
        return rect;
    }

    template <class T>
    [[nodiscard]] constexpr auto operator/(const Rect<T>& rect,
                                           const T scalar) noexcept
    {
        return Rect{
            rect.position / scalar,
            rect.size / scalar
        };
    }

    template <class T>
    constexpr auto& operator/=(Rect<T>& rect,
                               const T scalar) noexcept
    {
        rect.position /= scalar;
        rect.size /= scalar;
        return rect;
    }

    template <class T>
    [[nodiscard]] constexpr auto containsPoint(const Rect<T>& rect,
                                               const math::Vector<T, 2>& point) noexcept
    {
        return point.v[0] >= rect.position.v[0] && point.v[0] <= (rect.position.v[0] + rect.size.v[0]) &&
            point.v[1] >= rect.position.v[1] && point.v[1] <= (rect.position.v[1] + rect.size.v[1]);
    }

    template <class T>
    [[nodiscard]] constexpr auto contains(const Rect<T>& rect1,
                                          const Rect<T>& rect2) noexcept
    {
        return containsPoint(rect1, rect2.position.v[0], rect2.position.v[1]) &&
            containsPoint(rect1, rect2.position.v[0] + rect2.size.v[0], rect2.position.v[1] + rect2.size.v[1]);
    }

    template <class T>
    [[nodiscard]] constexpr auto intersects(const Rect<T>& rect1,
                                            const Rect<T>& rect2) noexcept
    {
        if (constexpr T t = rect2.position.v[0] - rect1.position.v[0]; t > rect1.size.v[0] || -t > rect2.size.v[0])
            return false;
        if (constexpr T t = rect2.position.v[1] - rect1.position.v[1]; t > rect1.size.v[1] || -t > rect2.size.v[1])
            return false;
        return true;
    }

    template <class T>
    constexpr void inflate(Rect<T>& rect,
                           const T horizontalAmount,
                           const T verticalAmount) noexcept
    {
        rect.position.v[0] -= horizontalAmount;
        rect.position.v[1] -= verticalAmount;
        rect.size.v[0] += horizontalAmount * T(2);
        rect.size.v[1] += verticalAmount * T(2);
    }

    template <class T>
    constexpr auto inflated(const Rect<T>& rect,
                           const T horizontalAmount,
                           const T verticalAmount) noexcept
    {
        return Rect<T>{
            rect.position.v[0] - horizontalAmount,
            rect.position.v[1] - verticalAmount,
            rect.size.v[0] + horizontalAmount * T(2),
            rect.size.v[1] + verticalAmount * T(2)
        };
    }
}

#endif // OUZEL_MATH_RECT_HPP
