// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Box2.hpp"

namespace ouzel
{
    void Box2::getCorners(Vector2* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis
        dst[0].set(min.x, min.y); // left-bottom
        dst[1].set(max.x, min.y); // right-bottom
        dst[2].set(max.x, max.y); // right-top
        dst[3].set(min.x, max.y); // left-top
    }

    void Box2::merge(const Box2& box)
    {
        // Calculate the new minimum point
        min.x = std::min(min.x, box.min.x);
        min.y = std::min(min.y, box.min.y);

        // Calculate the new maximum point
        max.x = std::max(max.x, box.max.x);
        max.y = std::max(max.y, box.max.y);
    }
}
