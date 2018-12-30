// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_BOX3_HPP
#define OUZEL_MATH_BOX3_HPP

#include <cstdint>
#include <limits>
#include "math/Size3.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Box2;

    class Box3 final
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

        explicit Box3(const Box2& box);
        Box3& operator=(const Box2& box);

        inline Vector3 getCenter()
        {
            return Vector3(0.5F * (min.v[0] + max.v[0]),
                           0.5F * (min.v[1] + max.v[1]),
                           0.5F * (min.v[2] + max.v[2]));
        }

        void getCorners(Vector3* dst) const;

        bool intersects(const Box3& aabb) const
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
            return min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2];
        }

        void insertPoint(const Vector2& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
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

        inline const Box3 operator+(const Vector2& v) const
        {
            Box3 result(*this);
            result += v;
            return result;
        }

        inline const Box3 operator+(const Vector3& v) const
        {
            Box3 result(*this);
            result += v;
            return result;
        }

        inline Box3& operator+=(const Vector2& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline Box3& operator+=(const Vector3& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const Box3 operator-(const Vector2& v) const
        {
            Box3 result(*this);
            result -= v;
            return result;
        }

        inline const Box3 operator-(const Vector3& v) const
        {
            Box3 result(*this);
            result -= v;
            return result;
        }

        inline Box3& operator-=(const Vector2& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Box3& operator-=(const Vector3& v)
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

#endif // OUZEL_MATH_BOX3_HPP
