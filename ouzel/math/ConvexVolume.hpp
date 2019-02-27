// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <vector>
#include "math/Box.hpp"
#include "math/Plane.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    template<class T> class ConvexVolume final
    {
    public:
        ConvexVolume()
        {
        }

        explicit ConvexVolume(const std::vector<Plane<T>>& initPlanes):
            planes(initPlanes)
        {
        }

        bool isPointInside(const Vector<4, T>& position) const
        {
            for (const Plane<T>& plane : planes)
                if (plane.dot(position) < 0)
                    return false;

            return true;
        }

        bool isSphereInside(const Vector<4, T>& position, T radius) const
        {
            for (const Plane<T>& plane : planes)
                if (plane.dot(position) < -radius)
                    return false;

            return true;
        }

        bool isBoxInside(const Box<3, T>& box) const
        {
            Vector<4, T> leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2], 1);
            Vector<4, T> leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2], 1);
            Vector<4, T> leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2], 1);
            Vector<4, T> leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2], 1);
            Vector<4, T> rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2], 1);
            Vector<4, T> rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2], 1);
            Vector<4, T> rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2], 1);
            Vector<4, T> rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2], 1);

            for (const Plane<T>& plane : planes)
                if (plane.dot(leftBottomBack) < 0 &&
                    plane.dot(leftBottomFront) < 0 &&
                    plane.dot(leftTopBack) < 0 &&
                    plane.dot(leftTopFront) < 0 &&
                    plane.dot(rightBottomBack) < 0 &&
                    plane.dot(rightBottomFront) < 0 &&
                    plane.dot(rightTopBack) < 0 &&
                    plane.dot(rightTopFront) < 0)
                    return false;

            return true;
        }

        std::vector<Plane<T>> planes;
    };

    using ConvexVolumeF = ConvexVolume<float>;
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
