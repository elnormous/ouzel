// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <type_traits>
#include <vector>
#include "math/Box.hpp"
#include "math/Plane.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    template <typename T> class ConvexVolume final
    {
    public:
        ConvexVolume() noexcept {}

        explicit ConvexVolume(const std::vector<Plane<T>>& initPlanes) noexcept:
            planes(initPlanes)
        {
        }

        template<std::size_t N, typename std::enable_if<N>= 3>::type* = nullptr>
        inline auto isPointInside(const Vector<N, T>& position) const noexcept
        {
            for (const Plane<T>& plane : planes)
                if (plane.dot(position) < T(0))
                    return false;

            return true;
        }

        template<std::size_t N, typename std::enable_if<N>= 3>::type* = nullptr>
        inline auto isSphereInside(const Vector<N, T>& position, const T radius) const noexcept
        {
            for (const Plane<T>& plane : planes)
                if (plane.dot(position) < -radius)
                    return false;

            return true;
        }

        auto isBoxInside(const Box<3, T>& box) const noexcept
        {
            const Vector<4, T> leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2], 1);
            const Vector<4, T> leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2], 1);
            const Vector<4, T> leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2], 1);
            const Vector<4, T> leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2], 1);
            const Vector<4, T> rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2], 1);
            const Vector<4, T> rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2], 1);
            const Vector<4, T> rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2], 1);
            const Vector<4, T> rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2], 1);

            for (const Plane<T>& plane : planes)
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

    using ConvexVolumeF = ConvexVolume<float>;
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
