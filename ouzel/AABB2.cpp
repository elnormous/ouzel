// Copyright (C) 2015 Elviss Strazdins
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
        set(box._min,box._max);
    }

    Vector2 AABB2::getCenter()
    {
        Vector2 center;
        center.x = 0.5f*(_min.x+_max.x);
        center.y = 0.5f*(_min.y+_max.y);
        
        return center;
    }

    void AABB2::getCorners(Vector2 *dst) const
    {
        assert(dst);
        
        // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
        // Left-top.
        dst[0].set(_min.x, _max.y);
        // Left-bottom.
        dst[1].set(_min.x, _min.y);
        // Right-bottom.
        dst[2].set(_max.x, _min.y);
        // Right-top.
        dst[3].set(_max.x, _max.y);
    }

    bool AABB2::intersects(const AABB2& aabb) const
    {
        return ((_min.x >= aabb._min.x && _min.x <= aabb._max.x) || (aabb._min.x >= _min.x && aabb._min.x <= _max.x)) &&
               ((_min.y >= aabb._min.y && _min.y <= aabb._max.y) || (aabb._min.y >= _min.y && aabb._min.y <= _max.y));
    }

    bool AABB2::containPoint(const Vector2& point) const
    {
        if (point.x < _min.x) return false;
        if (point.y < _min.y) return false;
        if (point.x > _max.x) return false;
        if (point.y > _max.y) return false;
        return true;
    }

    void AABB2::merge(const AABB2& box)
    {
        // Calculate the new minimum point.
        _min.x = std::min(_min.x, box._min.x);
        _min.y = std::min(_min.y, box._min.y);
        
        // Calculate the new maximum point.
        _max.x = std::max(_max.x, box._max.x);
        _max.y = std::max(_max.y, box._max.y);
    }

    void AABB2::set(const Vector2& min, const Vector2& max)
    {
        this->_min = min;
        this->_max = max;
    }

    void AABB2::reset()
    {
        _min.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        _max.set(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    }

    bool AABB2::isEmpty() const
    {
        return _min.x > _max.x || _min.y > _max.y;
    }

    void AABB2::updateMinMax(const Vector2* point, uint32_t num)
    {
        for (ssize_t i = 0; i < num; i++)
        {
            // Leftmost point.
            if (point[i].x < _min.x)
                _min.x = point[i].x;
            
            // Lowest point.
            if (point[i].y < _min.y)
                _min.y = point[i].y;
            
            // Rightmost point.
            if (point[i].x > _max.x)
                _max.x = point[i].x;
            
            // Highest point.
            if (point[i].y > _max.y)
                _max.y = point[i].y;
        }
    }
}
