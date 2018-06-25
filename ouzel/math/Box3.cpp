// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Box3.hpp"
#include "Box2.hpp"

namespace ouzel
{
    Box3::Box3(const Box2& box):
        min(box.min), max(box.max)
    {
    }

    Box3& Box3::operator=(const Box2& box)
    {
        min = box.min;
        max = box.max;

        return *this;
    }

    void Box3::getCorners(Vector3* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis
        // Left-bottom-front
        dst[0].set(min.x, min.y, min.z);
        // Right-bottom-front
        dst[1].set(max.x, min.y, min.z);
        // Right-top-front
        dst[2].set(max.x, max.y, min.z);
        // Left-top-front
        dst[3].set(min.x, max.y, min.z);
        // Left-bottom-back
        dst[0].set(min.x, min.y, max.z);
        // Right-bottom-back
        dst[1].set(max.x, min.y, max.z);
        // Right-top-back
        dst[2].set(max.x, max.y, max.z);
        // Left-top-back
        dst[3].set(min.x, max.y, max.z);
    }

    void Box3::merge(const Box3& box)
    {
        // Calculate the new minimum point
        min.x = std::min(min.x, box.min.x);
        min.y = std::min(min.y, box.min.y);
        min.z = std::min(min.z, box.min.z);

        // Calculate the new maximum point
        max.x = std::max(max.x, box.max.x);
        max.y = std::max(max.y, box.max.y);
        max.z = std::max(max.y, box.max.z);
    }
}
