// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include <limits>
#include <cassert>
#include "AABB2.h"

namespace ouzel
{
    AABB2::AABB2()
    {
        reset();
    }

    AABB2::AABB2(const Vector2& min, const Vector2& max)
    {
        set(min, max);
    }

    AABB2::AABB2(const AABB2& box)
    {
        set(box.min,box.max);
    }

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

    bool AABB2::intersects(const AABB2& aabb) const
    {
        return ((min.x >= aabb.min.x && min.x <= aabb.max.x) || (aabb.min.x >= min.x && aabb.min.x <= max.x)) &&
               ((min.y >= aabb.min.y && min.y <= aabb.max.y) || (aabb.min.y >= min.y && aabb.min.y <= max.y));
    }

    bool AABB2::containPoint(const Vector2& point) const
    {
        if (point.x < min.x) return false;
        if (point.y < min.y) return false;
        if (point.x > max.x) return false;
        if (point.y > max.y) return false;
        return true;
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

    void AABB2::set(const Vector2& min, const Vector2& max)
    {
        this->min = min;
        this->max = max;
    }

    void AABB2::reset()
    {
        min.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        max.set(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    }

    bool AABB2::isEmpty() const
    {
        return min.x > max.x || min.y > max.y;
    }

    void AABB2::updateMinMax(const Vector2* points, uint32_t num)
    {
        for (uint32_t i = 0; i < num; i++)
        {
            // Leftmost point.
            if (points[i].x < min.x)
                min.x = points[i].x;
            
            // Lowest point.
            if (points[i].y < min.y)
                min.y = points[i].y;
            
            // Rightmost point.
            if (points[i].x > max.x)
                max.x = points[i].x;
            
            // Highest point.
            if (points[i].y > max.y)
                max.y = points[i].y;
        }
    }
    
    void AABB2::insertPoint(const Vector2& point)
    {
        if (point.x < min.x) min.x = point.x;
        if (point.x > max.x) max.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.y > max.y) max.y = point.y;
    }
}
