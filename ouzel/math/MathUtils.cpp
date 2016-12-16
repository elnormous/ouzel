// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MathUtils.h"

namespace ouzel
{
    bool linesIntersect(const Vector2& p0, const Vector2& p1,
                        const Vector2& p2, const Vector2& p3)
    {
        Vector2 s1(p1.v[0] - p0.v[0], p1.v[1] - p0.v[1]), s2(p3.v[0] - p2.v[0], p3.v[1] - p2.v[1]);

        float s, t;
        s = (-s1.v[1] * (p0.v[0] - p2.v[0]) + s1.v[0] * (p0.v[1] - p2.v[1])) / (-s2.v[0] * s1.v[1] + s1.v[0] * s2.v[1]);
        t = ( s2.v[0] * (p0.v[1] - p2.v[1]) - s2.v[1] * (p0.v[0] - p2.v[0])) / (-s2.v[0] * s1.v[1] + s1.v[0] * s2.v[1]);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            // Collision detected
            return true;
        }

        return false; // No collision
    }
}
