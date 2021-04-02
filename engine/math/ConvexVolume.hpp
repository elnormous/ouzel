// Ouzel by Elviss Strazdins

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <type_traits>
#include <vector>
#include "Box.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

namespace ouzel
{
    template <typename T> class ConvexVolume final
    {
    public:
        ConvexVolume() noexcept {}

        explicit ConvexVolume(const std::vector<Plane<T>>& initPlanes) noexcept:
            planes{initPlanes}
        {
        }

        template<std::size_t N>
        std::enable_if_t<N >= 3, bool>
        isPointInside(const Vector<T, N>& position) const noexcept
        {
            for (const auto& plane : planes)
                if (plane.dot(position) < T(0))
                    return false;

            return true;
        }

        template<std::size_t N>
        std::enable_if_t<N >= 3, bool>
        isSphereInside(const Vector<T, N>& position, const T radius) const noexcept
        {
            for (const auto& plane : planes)
                if (plane.dot(position) < -radius)
                    return false;

            return true;
        }

        auto isBoxInside(const Box<T, 3>& box) const noexcept
        {
            const Vector<T, 3> leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2]);
            const Vector<T, 3> leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2]);
            const Vector<T, 3> leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2]);
            const Vector<T, 3> leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2]);
            const Vector<T, 3> rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2]);
            const Vector<T, 3> rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2]);
            const Vector<T, 3> rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2]);
            const Vector<T, 3> rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2]);

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

        std::vector<Plane<T>> planes;
    };
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
