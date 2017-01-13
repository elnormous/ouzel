// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include <limits>
#include "math/Vector3.h"
#include "math/Size3.h"

#pragma once

namespace ouzel
{
    class AABB2;
    
    class AABB3
    {
    public:
        Vector3 min;
        Vector3 max;

        AABB3():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        AABB3(const Vector3& aMin, const Vector3& aMax):
            min(aMin), max(aMax)
        {
        }

        AABB3(const AABB3& box):
            min(box.min), max(box.max)
        {
        }

        AABB3(const AABB2& box);
        AABB3& operator=(const AABB2& box);

        Vector3 getCenter()
        {
            return Vector3(0.5f * (min.v[0] + max.v[0]),
                           0.5f * (min.v[1] + max.v[1]),
                           0.5f * (min.v[2] + max.v[2]));
        }

        void getCorners(Vector3* dst) const;

        bool intersects(const AABB3& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1] ||
                     aabb.min.v[2] > max.v[2] ||
                     aabb.max.v[2] < min.v[2]);
        }

        bool containsPoint(const Vector3& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[2] < min.v[2]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            if (point.v[2] > max.v[2]) return false;
            return true;
        }

        void merge(const AABB3& box);

        void set(const Vector2& newMin, const Vector2& newMax)
        {
            min = newMin;
            max = newMax;
        }

        void reset()
        {
            min.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
            max.set(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2];
        }

        void insertPoint(const Vector3& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
            if (point.v[2] < min.v[2]) min.v[2] = point.v[2];
            if (point.v[2] > max.v[2]) max.v[2] = point.v[2];
        }

        inline AABB3 operator+(const Vector3& v) const
        {
            AABB3 result(*this);
            result += v;
            return result;
        }

        inline AABB3& operator+=(const Vector3& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline AABB3 operator-(const Vector3& v) const
        {
            AABB3 result(*this);
            result -= v;
            return result;
        }

        inline AABB3& operator-=(const Vector3& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size3 getSize() const
        {
            return Size3(max.v[0] - min.v[0], max.v[1] - min.v[1], max.v[2] - min.v[2]);
        }
    };
}
