// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_CONVEXVOLUME_HPP
#define OUZEL_MATH_CONVEXVOLUME_HPP

#include <vector>
#include "math/Box3.hpp"
#include "math/Plane.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    class ConvexVolume final
    {
    public:
        ConvexVolume()
        {
        }

        explicit ConvexVolume(const std::vector<Plane>& initPlanes):
            planes(initPlanes)
        {
        }

        bool isPointInside(const Vector4& position) const
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < 0.0F)
                    return false;
            }

            return true;
        }

        bool isSphereInside(const Vector4& position, float radius) const
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < -radius)
                    return false;
            }

            return true;
        }

        bool isBoxInside(const Box3& box) const
        {
            Vector4 leftBottomBack(box.min.x, box.min.y, box.min.z, 1.0F);
            Vector4 leftBottomFront(box.min.x, box.min.y, box.max.z, 1.0F);
            Vector4 leftTopBack(box.min.x, box.max.y, box.min.z, 1.0F);
            Vector4 leftTopFront(box.min.x, box.max.y, box.max.z, 1.0F);
            Vector4 rightBottomBack(box.max.x, box.min.y, box.min.z, 1.0F);
            Vector4 rightBottomFront(box.max.x, box.min.y, box.max.z, 1.0F);
            Vector4 rightTopBack(box.max.x, box.max.y, box.min.z, 1.0F);
            Vector4 rightTopFront(box.max.x, box.max.y, box.max.z, 1.0F);

            for (const Plane& plane : planes)
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

        std::vector<Plane> planes;
    };
}

#endif // OUZEL_MATH_CONVEXVOLUME_HPP
