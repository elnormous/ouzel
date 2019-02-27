// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_BOX_HPP
#define OUZEL_MATH_BOX_HPP

#include <cstdint>
#include <limits>
#include "math/Size.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    template<class T> class Box2 final
    {
    public:
        Vector<2, T> min;
        Vector<2, T> max;

        Box2():
            min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),
            max(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
        {
        }

        Box2(const Vector<2, T>& initMin, const Vector<2, T>& initMax):
            min(initMin), max(initMax)
        {
        }

        Box2(const Box2& box):
            min(box.min), max(box.max)
        {
        }

        inline Vector<2, T> getCenter()
        {
            return Vector<2, T>((min.v[0] + max.v[0]) / 2,
                                (min.v[1] + max.v[1]) / 2);
        }

        void getCorners(Vector<2, T>* dst) const;

        bool intersects(const Box2& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1]);
        }

        bool containsPoint(const Vector<2, T>& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            return true;
        }

        void merge(const Box2& box);

        void reset()
        {
            min.v[0] = min.v[1] = std::numeric_limits<T>::max();
            max.v[0] = max.v[1] = std::numeric_limits<T>::lowest();
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1];
        }

        void insertPoint(const Vector<2, T>& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
        }

        inline const Box2 operator+(const Vector<2, T>& v) const
        {
            Box2 result(*this);
            result += v;
            return result;
        }

        inline Box2& operator+=(const Vector<2, T>& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const Box2 operator-(const Vector<2, T>& v) const
        {
            Box2 result(*this);
            result -= v;
            return result;
        }

        inline Box2& operator-=(const Vector<2, T>& v)
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

    using Box2F = Box2<float>;

    template<class T> class Box3 final
    {
    public:
        Vector<3, T> min;
        Vector<3, T> max;

        Box3():
            min(std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max()),
            max(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest())
        {
        }

        Box3(const Vector<3, T>& initMin, const Vector<3, T>& initMax):
            min(initMin), max(initMax)
        {
        }

        Box3(const Box3& box):
            min(box.min), max(box.max)
        {
        }

        explicit Box3(const Box2<T>& box);

        inline Vector<3, T> getCenter()
        {
            return Vector<3, T>((min.v[0] + max.v[0]) / 2,
                                (min.v[1] + max.v[1]) / 2,
                                (min.v[2] + max.v[2]) / 2);
        }

        void getCorners(Vector<3, T>* dst) const;

        bool intersects(const Box3& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1] ||
                     aabb.min.v[2] > max.v[2] ||
                     aabb.max.v[2] < min.v[2]);
        }

        bool containsPoint(const Vector<3, T>& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[2] < min.v[2]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            if (point.v[2] > max.v[2]) return false;
            return true;
        }

        void merge(const Box3<T>& box);

        void reset()
        {
            min.v[0] = min.v[1] = min.v[2] = std::numeric_limits<T>::max();
            max.v[0] = max.v[1] = max.v[2] = std::numeric_limits<T>::lowest();
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2];
        }

        void insertPoint(const Vector<2, T>& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
        }

        void insertPoint(const Vector<3, T>& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
            if (point.v[2] < min.v[2]) min.v[2] = point.v[2];
            if (point.v[2] > max.v[2]) max.v[2] = point.v[2];
        }

        inline const Box3 operator+(const Vector<3, T>& v) const
        {
            Box3 result(*this);
            result += v;
            return result;
        }

        inline Box3& operator+=(const Vector<3, T>& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const Box3 operator-(const Vector<3, T>& v) const
        {
            Box3 result(*this);
            result -= v;
            return result;
        }

        inline Box3& operator-=(const Vector<3, T>& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size3<T> getSize() const
        {
            return Size3<T>(max.v[0] - min.v[0], max.v[1] - min.v[1], max.v[2] - min.v[2]);
        }
    };

    using Box3F = Box3<float>;
}

#endif // OUZEL_MATH_BOX_HPP
