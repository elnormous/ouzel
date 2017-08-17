// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Box2.hpp"

namespace ouzel
{
    void Box2::getCorners(Vector2* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-bottom.
        dst[0].set(min.x, min.y);
        // Right-bottom.
        dst[1].set(max.x, min.y);
        // Right-top.
        dst[2].set(max.x, max.y);
        // Left-top.
        dst[3].set(min.x, max.y);
    }

    void Box2::merge(const Box2& box)
    {
        // Calculate the new minimum point.
        min.x = std::min(min.x, box.min.x);
        min.y = std::min(min.y, box.min.y);

        // Calculate the new maximum point.
        max.x = std::max(max.x, box.max.x);
        max.y = std::max(max.y, box.max.y);
    }
}
