// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include <cstdlib>
#include <memory>
#include "MathUtils.h"

namespace ouzel
{
    uint32_t calculateMipLevels(uint32_t width, uint32_t height)
    {
        uint32_t result = 1;

        while (width > 1 || height > 1)
        {
            width >>= 1;
            height >>= 1;

            ++result;
        }

        return result;
    }

    bool linesIntersect(const Vector2& p0, const Vector2& p1,
                        const Vector2& p2, const Vector2& p3)
    {
        Vector2 s1(p1.x - p0.x, p1.y - p0.y), s2(p3.x - p2.x, p3.y - p2.y);

        float s, t;
        s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
        t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            // Collision detected
            return true;
        }

        return false; // No collision
    }
}
