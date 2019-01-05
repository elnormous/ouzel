// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_BOX2_HPP
#define OUZEL_MATH_BOX2_HPP

#include <cstdint>
#include <limits>
#include "math/Vector2.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    template<class T> class Box2 final
    {
    public:
        Vector2<T> min;
        Vector2<T> max;

        Box2():
            min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),
            max(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
        {
        }

        Box2(const Vector2<T>& initMin, const Vector2<T>& initMax):
            min(initMin), max(initMax)
        {
        }

        Box2(const Box2& box):
            min(box.min), max(box.max)
        {
        }

        inline Vector2<T> getCenter()
        {
            return Vector2<T>((min.v[0] + max.v[0]) / 2,
                              (min.v[1] + max.v[1]) / 2);
        }

        void getCorners(Vector2<T>* dst) const;

        bool intersects(const Box2& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1]);
        }

        bool containsPoint(const Vector2<T>& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            return true;
        }

        void merge(const Box2& box);

        void set(const Vector2<T>& newMin, const Vector2<T>& newMax)
        {
            min = newMin;
            max = newMax;
        }

        void reset()
        {
            min.set(std::numeric_limits<T>::max(), std::numeric_limits<T>::max());
            max.set(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest());
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1];
        }

        void insertPoint(const Vector2<T>& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
        }

        inline const Box2 operator+(const Vector2<T>& v) const
        {
            Box2 result(*this);
            result += v;
            return result;
        }

        inline Box2& operator+=(const Vector2<T>& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const Box2 operator-(const Vector2<T>& v) const
        {
            Box2 result(*this);
            result -= v;
            return result;
        }

        inline Box2& operator-=(const Vector2<T>& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size2<T> getSize() const
        {
            return Size2<T>(max.v[0] - min.v[0], max.v[1] - min.v[1]);
        }
    };
}

#endif // OUZEL_MATH_BOX2_HPP
