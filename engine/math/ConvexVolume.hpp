// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <type_traits>
#include <vector>
#include "Box.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

namespace ouzel::math
{
    template <typename T> class ConvexVolume final
    {
    public:
        std::vector<Plane<T>> planes;

        ConvexVolume() noexcept {}

        explicit ConvexVolume(const std::vector<Plane<T>>& initPlanes) noexcept:
            planes{initPlanes}
        {
        }

        template<std::size_t n>
        [[nodiscard]] bool isPointInside(const math::Vector<T, n>& position) const noexcept
        {
            static_assert(n >= 3);

            for (const auto& plane : planes)
                if (plane.dot(position) < T(0))
                    return false;

            return true;
        }

        template<std::size_t n>
        [[nodiscard]] bool isSphereInside(const math::Vector<T, n>& position, const T radius) const noexcept
        {
            static_assert(n >= 3);

            for (const auto& plane : planes)
                if (plane.dot(position) < -radius)
                    return false;

            return true;
        }

        [[nodiscard]] auto isBoxInside(const math::Box<T, 3>& box) const noexcept
        {
            const math::Vector<T, 3> leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2]);
            const math::Vector<T, 3> leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2]);
            const math::Vector<T, 3> leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2]);
            const math::Vector<T, 3> leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2]);
            const math::Vector<T, 3> rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2]);
            const math::Vector<T, 3> rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2]);
            const math::Vector<T, 3> rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2]);
            const math::Vector<T, 3> rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2]);

            for (const auto& plane : planes)
                if (plane.dot(leftBottomBack) < T(0) &&
                    plane.dot(leftBottomFront) < T(0) &&
                    plane.dot(leftTopBack) < T(0) &&
                    plane.dot(leftTopFront) < T(0) &&
                    plane.dot(rightBottomBack) < T(0) &&
                    plane.dot(rightBottomFront) < T(0) &&
                    plane.dot(rightTopBack) < T(0) &&
                    plane.dot(rightTopFront) < T(0))
                    return false;

            return true;
        }
    };
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
