// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Box2.hpp"

namespace ouzel
{
    void Box2::getCorners(Vector2* dst) const
    {
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis
        dst[0].set(min.v[0], min.v[1]); // left-bottom
        dst[1].set(max.v[0], min.v[1]); // right-bottom
        dst[2].set(max.v[0], max.v[1]); // right-top
        dst[3].set(min.v[0], max.v[1]); // left-top
    }

    void Box2::merge(const Box2& box)
    {
        // Calculate the new minimum point
        min.v[0] = std::min(min.v[0], box.min.v[0]);
        min.v[1] = std::min(min.v[1], box.min.v[1]);

        // Calculate the new maximum point
        max.v[0] = std::max(max.v[0], box.max.v[0]);
        max.v[1] = std::max(max.v[1], box.max.v[1]);
    }
}
