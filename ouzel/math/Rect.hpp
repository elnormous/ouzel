// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_RECT_HPP
#define OUZEL_MATH_RECT_HPP

#include "math/Vector2.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Rect final
    {
    public:
        Vector2 position;
        Size2 size;

        Rect()
        {
        }

        Rect(float width, float height):
            size(width, height)
        {
        }

        Rect(float initX, float initY, float width, float height):
            position(initX, initY), size(width, height)
        {
        }

        Rect(const Vector2& initPosition, float width, float height):
            position(initPosition), size(width, height)
        {
        }

        Rect(const Vector2& initPosition, const Size2& initSize):
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

        void set(float newX, float newY, float newWidth, float newHeight)
        {
            position.v[0] = newX;
            position.v[1] = newY;
            size.width = newWidth;
            size.height = newHeight;
        }

        void set(const Vector2& newPosition, float newWidth, float newHeight)
        {
            position = newPosition;
            size.width = newWidth;
            size.height = newHeight;
        }

        void setPosition(float newX, float newY)
        {
            position.v[0] = newX;
            position.v[1] = newY;
        }

        void setPosition(const Vector2& newPosition)
        {
            position = newPosition;
        }

        float left() const
        {
            return position.v[0];
        }

        float bottom() const
        {
            return position.v[1];
        }

        float right() const
        {
            return position.v[0] + size.width;
        }

        float top() const
        {
            return position.v[1] + size.height;
        }

        Vector2 bottomLeft() const
        {
            return position;
        }

        Vector2 topRight() const
        {
            return Vector2(position.v[0] + size.width, position.v[1] + size.height);
        }

        bool containsPoint(float x, float y) const
        {
            return x >= position.v[0] && x <= (position.v[0] + size.width) &&
                y >= position.v[1] && y <= (position.v[1] + size.height);
        }

        bool containsPoint(const Vector2& point) const
        {
            return point.v[0] >= position.v[0] && point.v[0] <= (position.v[0] + size.width) &&
                point.v[1] >= position.v[1] && point.v[1] <= (position.v[1] + size.height);
        }

        bool contains(float x, float y, float width, float height) const
        {
            return containsPoint(x, y) && containsPoint(x + width, y + height);
        }

        bool contains(const Rect& r) const
        {
            return contains(r.position.v[0], r.position.v[1], r.size.width, r.size.height);
        }

        bool intersects(float x, float y, float width, float height) const
        {
            float t;
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

        void inflate(float horizontalAmount, float verticalAmount)
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

        inline const Rect operator*(float scalar) const
        {
            return Rect(position.v[0] * scalar, position.v[1] * scalar,
                        size.width * scalar, size.height * scalar);
        }

        inline Rect& operator*=(float scalar)
        {
            position.v[0] *= scalar;
            position.v[1] *= scalar;
            size.width *= scalar;
            size.height *= scalar;
            return *this;
        }

        inline const Rect operator/(float scalar) const
        {
            return Rect(position.v[0] / scalar, position.v[1] / scalar,
                        size.width / scalar, size.height / scalar);
        }

        inline Rect& operator/=(float scalar)
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
