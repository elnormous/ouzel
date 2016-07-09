// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <limits>
#include <cassert>
#include "AABB2.h"

namespace ouzel
{
    Vector2 AABB2::getCenter()
    {
        Vector2 center;
        center.x = 0.5f * (min.x + max.x);
        center.y = 0.5f * (min.y + max.y);

        return center;
    }

    void AABB2::getCorners(Vector2* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-top.
        dst[0].set(min.x, max.y);
        // Left-bottom.
        dst[1].set(min.x, min.y);
        // Right-bottom.
        dst[2].set(max.x, min.y);
        // Right-top.
        dst[3].set(max.x, max.y);
    }

    void AABB2::merge(const AABB2& box)
    {
        // Calculate the new minimum point.
        min.x = std::min(min.x, box.min.x);
        min.y = std::min(min.y, box.min.y);

        // Calculate the new maximum point.
        max.x = std::max(max.x, box.max.x);
        max.y = std::max(max.y, box.max.y);
    }


    void AABB2::reset()
    {
        min.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        max.set(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    }
}
