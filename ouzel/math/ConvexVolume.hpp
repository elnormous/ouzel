// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <vector>
#include "math/Box3.hpp"
#include "math/Plane.hpp"
#include "math/Vector4.hpp"

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

        bool isPointInside(const Vector4<T>& position) const
        {
            for (const Plane<T>& plane : planes)
            {
                if (plane.dot(position) < 0.0F)
                    return false;
            }

            return true;
        }

        bool isSphereInside(const Vector4<T>& position, float radius) const
        {
            for (const Plane<T>& plane : planes)
            {
                if (plane.dot(position) < -radius)
                    return false;
            }

            return true;
        }

        bool isBoxInside(const Box3<T>& box) const
        {
            Vector4<T> leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2], 1.0F);
            Vector4<T> leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2], 1.0F);
            Vector4<T> leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2], 1.0F);
            Vector4<T> leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2], 1.0F);
            Vector4<T> rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2], 1.0F);
            Vector4<T> rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2], 1.0F);
            Vector4<T> rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2], 1.0F);
            Vector4<T> rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2], 1.0F);

            for (const Plane<T>& plane : planes)
            {
                if (plane.dot(leftBottomBack) < 0.0F &&
                    plane.dot(leftBottomFront) < 0.0F &&
                    plane.dot(leftTopBack) < 0.0F &&
                    plane.dot(leftTopFront) < 0.0F &&
                    plane.dot(rightBottomBack) < 0.0F &&
                    plane.dot(rightBottomFront) < 0.0F &&
                    plane.dot(rightTopBack) < 0.0F &&
                    plane.dot(rightTopFront) < 0.0F)
                    return false;
            }

            return true;
        }

        std::vector<Plane<T>> planes;
    };
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
