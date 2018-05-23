// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include <limits>
#include "math/Vector3.hpp"
#include "math/Size3.hpp"

#pragma once

namespace ouzel
{
    class Box2;

    class Box3
    {
    public:
        Vector3 min;
        Vector3 max;

        Box3():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        Box3(const Vector3& initMin, const Vector3& initMax):
            min(initMin), max(initMax)
        {
        }

        Box3(const Box3& box):
            min(box.min), max(box.max)
        {
        }

        Box3(const Box2& box);
        Box3& operator=(const Box2& box);

        inline Vector3 getCenter()
        {
            return Vector3(0.5F * (min.x + max.x),
                           0.5F * (min.y + max.y),
                           0.5F * (min.z + max.z));
        }

        void getCorners(Vector3* dst) const;

        bool intersects(const Box3& aabb) const
        {
            return !(aabb.min.x > max.x ||
                     aabb.max.x < min.x ||
                     aabb.min.y > max.y ||
                     aabb.max.y < min.y ||
                     aabb.min.z > max.z ||
                     aabb.max.z < min.z);
        }

        bool containsPoint(const Vector3& point) const
        {
            if (point.x < min.x) return false;
            if (point.y < min.y) return false;
            if (point.z < min.z) return false;
            if (point.x > max.x) return false;
            if (point.y > max.y) return false;
            if (point.z > max.z) return false;
            return true;
        }

        void merge(const Box3& box);

        void set(const Vector3& newMin, const Vector3& newMax)
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
            return min.x > max.x || min.y > max.y || min.z > max.z;
        }

        void insertPoint(const Vector3& point)
        {
            if (point.x < min.x) min.x = point.x;
            if (point.x > max.x) max.x = point.x;
            if (point.y < min.y) min.y = point.y;
            if (point.y > max.y) max.y = point.y;
            if (point.z < min.z) min.z = point.z;
            if (point.z > max.z) max.z = point.z;
        }

        inline Box3 operator+(const Vector3& v) const
        {
            Box3 result(*this);
            result += v;
            return result;
        }

        inline Box3& operator+=(const Vector3& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline Box3 operator-(const Vector3& v) const
        {
            Box3 result(*this);
            result -= v;
            return result;
        }

        inline Box3& operator-=(const Vector3& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size3 getSize() const
        {
            return Size3(max.x - min.x, max.y - min.y, max.z - min.z);
        }
    };
}
