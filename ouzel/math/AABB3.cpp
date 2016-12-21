// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "AABB3.h"

namespace ouzel
{
    void AABB3::getCorners(Vector3* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-bottom-front.
        dst[0].set(min.v[0], min.v[1], min.v[2]);
        // Right-bottom-front.
        dst[1].set(max.v[0], min.v[1], min.v[2]);
        // Right-top-front.
        dst[2].set(max.v[0], max.v[1], min.v[2]);
        // Left-top-front.
        dst[3].set(min.v[0], max.v[1], min.v[2]);
        // Left-bottom-back.
        dst[0].set(min.v[0], min.v[1], max.v[2]);
        // Right-bottom-back.
        dst[1].set(max.v[0], min.v[1], max.v[2]);
        // Right-top-back.
        dst[2].set(max.v[0], max.v[1], max.v[2]);
        // Left-top-back.
        dst[3].set(min.v[0], max.v[1], max.v[2]);
    }

    void AABB3::merge(const AABB3& box)
    {
        // Calculate the new minimum point.
        min.v[0] = std::min(min.v[0], box.min.v[0]);
        min.v[1] = std::min(min.v[1], box.min.v[1]);
        min.v[2] = std::min(min.v[2], box.min.v[2]);

        // Calculate the new maximum point.
        max.v[0] = std::max(max.v[0], box.max.v[0]);
        max.v[1] = std::max(max.v[1], box.max.v[1]);
        max.v[2] = std::max(max.v[1], box.max.v[2]);
    }
}
