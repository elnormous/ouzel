// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <vector>
#include "math/Box3.hpp"
#include "math/Plane.hpp"
#include "math/Vector4.hpp"

namespace ouzel
{
    class ConvexVolume
    {
    public:
        ConvexVolume()
        {
        }

        ConvexVolume(const std::vector<Plane>& initPlanes):
            planes(initPlanes)
        {
        }

        bool isPointInside(const Vector4& position)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < 0.0F)
                    return false;
            }

            return true;
        }

        bool isSphereInside(const Vector4& position, float radius)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < -radius)
                    return false;
            }

            return true;
        }

        bool isBoxInside(const Box3& box)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(Vector4(box.min.x, box.min.y, box.min.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.max.x, box.min.y, box.min.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.min.x, box.max.y, box.min.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.min.x, box.min.y, box.max.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.max.x, box.max.y, box.min.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.max.x, box.min.y, box.max.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.min.x, box.max.y, box.max.z, 1.0F)) >= 0.0F)
                    continue;

                if (plane.dot(Vector4(box.max.x, box.max.y, box.max.z, 1.0F)) >= 0.0F)
                    continue;

                return false;
            }

            return true;
        }

        std::vector<Plane> planes;
    };
}
