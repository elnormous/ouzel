// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include <limits>
#include "math/Vector2.h"
#include "math/Size2.h"

#pragma once

namespace ouzel
{
    class AABB2
    {
    public:
        Vector2 min;
        Vector2 max;

        AABB2():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        AABB2(const Vector2& aMin, const Vector2& aMax):
            min(aMin), max(aMax)
        {
        }

        AABB2(const AABB2& box):
            min(box.min), max(box.max)
        {
        }

        Vector2 getCenter()
        {
            return Vector2(0.5f * (min.v[0] + max.v[0]),
                           0.5f * (min.v[1] + max.v[1]));
        }

        void getCorners(Vector2* dst) const;

        bool intersects(const AABB2& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1]);
        }

        bool containsPoint(const Vector2& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            return true;
        }

        void merge(const AABB2& box);

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
            return min.v[0] > max.v[0] || min.v[1] > max.v[1];
        }

        void insertPoint(const Vector2& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
        }

        inline AABB2 operator+(const Vector2& v) const
        {
            AABB2 result(*this);
            result += v;
            return result;
        }

        inline AABB2& operator+=(const Vector2& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline AABB2 operator-(const Vector2& v) const
        {
            AABB2 result(*this);
            result -= v;
            return result;
        }

        inline AABB2& operator-=(const Vector2& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size2 getSize() const
        {
            return Size2(max.v[0] - min.v[0], max.v[1] - min.v[1]);
        }
    };
}
