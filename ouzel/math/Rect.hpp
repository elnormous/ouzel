// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RECT_HPP
#define RECT_HPP

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
            position.x = newX;
            position.y = newY;
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
            position.x = newX;
            position.y = newY;
        }

        void setPosition(const Vector2& newPosition)
        {
            position = newPosition;
        }

        float left() const
        {
            return position.x;
        }

        float bottom() const
        {
            return position.y;
        }

        float right() const
        {
            return position.x + size.width;
        }

        float top() const
        {
            return position.y + size.height;
        }

        Vector2 bottomLeft() const
        {
            return position;
        }

        Vector2 topRight() const
        {
            return Vector2(position.x + size.width, position.y + size.height);
        }

        bool containsPoint(float x, float y) const
        {
            return x >= position.x && x <= (position.x + size.width) &&
                y >= position.y && y <= (position.y + size.height);
        }

        bool containsPoint(const Vector2& point) const
        {
            return point.x >= position.x && point.x <= (position.x + size.width) &&
                point.y >= position.y && point.y <= (position.y + size.height);
        }

        bool contains(float x, float y, float width, float height) const
        {
            return containsPoint(x, y) && containsPoint(x + width, y + height);
        }

        bool contains(const Rect& r) const
        {
            return contains(r.position.x, r.position.y, r.size.width, r.size.height);
        }

        bool intersects(float x, float y, float width, float height) const
        {
            float t;
            if ((t = x - position.x) > size.width || -t > width)
                return false;
            if ((t = y - position.y) > size.height || -t > height)
                return false;
            return true;
        }

        bool intersects(const Rect& r) const
        {
            return intersects(r.position.x, r.position.y, r.size.width, r.size.height);
        }

        static bool intersect(const Rect& r1, const Rect& r2, Rect& dst);

        static void combine(const Rect& r1, const Rect& r2, Rect& dst);

        void inflate(float horizontalAmount, float verticalAmount)
        {
            position.x -= horizontalAmount;
            position.y -= verticalAmount;
            size.width += horizontalAmount * 2;
            size.height += verticalAmount * 2;
        }

        Rect& operator=(const Rect& r)
        {
            position.x = r.position.x;
            position.y = r.position.y;
            size.width = r.size.width;
            size.height = r.size.height;
            return *this;
        }

        bool operator==(const Rect& r) const
        {
            return position.x == r.position.x && size.width == r.size.width &&
                position.y == r.position.y && size.height == r.size.height;
        }

        bool operator!=(const Rect& r) const
        {
            return position.x != r.position.x || size.width != r.size.width ||
                position.y != r.position.y || size.height != r.size.height;
        }

        inline const Rect operator*(float scalar) const
        {
            return Rect(position.x * scalar, position.y * scalar,
                        size.width * scalar, size.height * scalar);
        }

        inline Rect& operator*=(float scalar)
        {
            position.x *= scalar;
            position.y *= scalar;
            size.width *= scalar;
            size.height *= scalar;
            return *this;
        }

        inline const Rect operator/(float scalar) const
        {
            return Rect(position.x / scalar, position.y / scalar,
                        size.width / scalar, size.height / scalar);
        }

        inline Rect& operator/=(float scalar)
        {
            position.x /= scalar;
            position.y /= scalar;
            size.width /= scalar;
            size.height /= scalar;
            return *this;
        }
    };
}

#endif // RECT_HPP
