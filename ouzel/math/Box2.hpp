// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <limits>
#include "math/Vector2.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Box2 final
    {
    public:
        Vector2 min;
        Vector2 max;

        Box2():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        Box2(const Vector2& initMin, const Vector2& initMax):
            min(initMin), max(initMax)
        {
        }

        Box2(const Box2& box):
            min(box.min), max(box.max)
        {
        }

        inline Vector2 getCenter()
        {
            return Vector2(0.5F * (min.x + max.x),
                           0.5F * (min.y + max.y));
        }

        void getCorners(Vector2* dst) const;

        bool intersects(const Box2& aabb) const
        {
            return !(aabb.min.x > max.x ||
                     aabb.max.x < min.x ||
                     aabb.min.y > max.y ||
                     aabb.max.y < min.y);
        }

        bool containsPoint(const Vector2& point) const
        {
            if (point.x < min.x) return false;
            if (point.y < min.y) return false;
            if (point.x > max.x) return false;
            if (point.y > max.y) return false;
            return true;
        }

        void merge(const Box2& box);

        void set(const Vector2& newMin, const Vector2& newMax)
        {
            min = newMin;
            max = newMax;
        }

        void reset()
        {
            min.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            max.set(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        }

        bool isEmpty() const
        {
            return min.x > max.x || min.y > max.y;
        }

        void insertPoint(const Vector2& point)
        {
            if (point.x < min.x) min.x = point.x;
            if (point.x > max.x) max.x = point.x;
            if (point.y < min.y) min.y = point.y;
            if (point.y > max.y) max.y = point.y;
        }

        inline const Box2 operator+(const Vector2& v) const
        {
            Box2 result(*this);
            result += v;
            return result;
        }

        inline Box2& operator+=(const Vector2& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const Box2 operator-(const Vector2& v) const
        {
            Box2 result(*this);
            result -= v;
            return result;
        }

        inline Box2& operator-=(const Vector2& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size2 getSize() const
        {
            return Size2(max.x - min.x, max.y - min.y);
        }
    };
}
