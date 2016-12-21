// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "AABB2.h"

namespace ouzel
{
    void AABB2::getCorners(Vector2* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-bottom.
        dst[0].set(min.v[0], min.v[1]);
        // Right-bottom.
        dst[1].set(max.v[0], min.v[1]);
        // Right-top.
        dst[2].set(max.v[0], max.v[1]);
        // Left-top.
        dst[3].set(min.v[0], max.v[1]);
    }

    void AABB2::merge(const AABB2& box)
    {
        // Calculate the new minimum point.
        min.v[0] = std::min(min.v[0], box.min.v[0]);
        min.v[1] = std::min(min.v[1], box.min.v[1]);

        // Calculate the new maximum point.
        max.v[0] = std::max(max.v[0], box.max.v[0]);
        max.v[1] = std::max(max.v[1], box.max.v[1]);
    }
}
