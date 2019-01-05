// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_RECT_HPP
#define OUZEL_MATH_RECT_HPP

#include "math/Vector2.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    template<class T> class Rect final
    {
    public:
        Vector2<T> position;
        Size2<T> size;

        Rect()
        {
        }

        Rect(T width, T height):
            size(width, height)
        {
        }

        Rect(T x, T y, T width, T height):
            position(x, y), size(width, height)
        {
        }

        Rect(const Vector2<T>& initPosition, T width, T height):
            position(initPosition), size(width, height)
        {
        }

        Rect(const Vector2<T>& initPosition, const Size2<T>& initSize):
            position(initPosition), size(initSize)
        {
        }

        Rect(const Rect& copy):
            position(copy.position), size(copy.size)
        {
        }

        inline bool isEmpty() const
        {
            return size.isZero();
        }

        void set(T x, T y, T width, T height)
        {
            position.v[0] = x;
            position.v[1] = y;
            size.width = width;
            size.height = height;
        }

        void set(const Vector2<T>& newPosition, T width, T height)
        {
            position = newPosition;
            size.width = width;
            size.height = height;
        }

        void setPosition(T x, T y)
        {
            position.v[0] = x;
            position.v[1] = y;
        }

        void setPosition(const Vector2<T>& newPosition)
        {
            position = newPosition;
        }

        T left() const
        {
            return position.v[0];
        }

        T bottom() const
        {
            return position.v[1];
        }

        T right() const
        {
            return position.v[0] + size.width;
        }

        T top() const
        {
            return position.v[1] + size.height;
        }

        Vector2<T> bottomLeft() const
        {
            return position;
        }

        Vector2<T> topRight() const
        {
            return Vector2<T>(position.v[0] + size.width, position.v[1] + size.height);
        }

        bool containsPoint(T x, T y) const
        {
            return x >= position.v[0] && x <= (position.v[0] + size.width) &&
                y >= position.v[1] && y <= (position.v[1] + size.height);
        }

        bool containsPoint(const Vector2<T>& point) const
        {
            return point.v[0] >= position.v[0] && point.v[0] <= (position.v[0] + size.width) &&
                point.v[1] >= position.v[1] && point.v[1] <= (position.v[1] + size.height);
        }

        bool contains(T x, T y, T width, T height) const
        {
            return containsPoint(x, y) && containsPoint(x + width, y + height);
        }

        bool contains(const Rect& r) const
        {
            return contains(r.position.v[0], r.position.v[1], r.size.width, r.size.height);
        }

        bool intersects(T x, T y, T width, T height) const
        {
            T t;
            if ((t = x - position.v[0]) > size.width || -t > width)
                return false;
            if ((t = y - position.v[1]) > size.height || -t > height)
                return false;
            return true;
        }

        bool intersects(const Rect& r) const
        {
            return intersects(r.position.v[0], r.position.v[1], r.size.width, r.size.height);
        }

        static bool intersect(const Rect& r1, const Rect& r2, Rect& dst);

        static void combine(const Rect& r1, const Rect& r2, Rect& dst);

        void inflate(T horizontalAmount, T verticalAmount)
        {
            position.v[0] -= horizontalAmount;
            position.v[1] -= verticalAmount;
            size.width += horizontalAmount * 2;
            size.height += verticalAmount * 2;
        }

        Rect& operator=(const Rect& other)
        {
            position.v[0] = other.position.v[0];
            position.v[1] = other.position.v[1];
            size.width = other.size.width;
            size.height = other.size.height;
            return *this;
        }

        bool operator==(const Rect& other) const
        {
            return position.v[0] == other.position.v[0] && size.width == other.size.width &&
                position.v[1] == other.position.v[1] && size.height == other.size.height;
        }

        bool operator!=(const Rect& other) const
        {
            return position.v[0] != other.position.v[0] || size.width != other.size.width ||
                position.v[1] != other.position.v[1] || size.height != other.size.height;
        }

        inline const Rect operator*(T scalar) const
        {
            return Rect(position.v[0] * scalar, position.v[1] * scalar,
                        size.width * scalar, size.height * scalar);
        }

        inline Rect& operator*=(T scalar)
        {
            position.v[0] *= scalar;
            position.v[1] *= scalar;
            size.width *= scalar;
            size.height *= scalar;
            return *this;
        }

        inline const Rect operator/(T scalar) const
        {
            return Rect(position.v[0] / scalar, position.v[1] / scalar,
                        size.width / scalar, size.height / scalar);
        }

        inline Rect& operator/=(T scalar)
        {
            position.v[0] /= scalar;
            position.v[1] /= scalar;
            size.width /= scalar;
            size.height /= scalar;
            return *this;
        }
    };
}

#endif // OUZEL_MATH_RECT_HPP
