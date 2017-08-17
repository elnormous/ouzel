// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Vector2.hpp"
#include "math/Size2.hpp"

namespace ouzel
{
    class Rectangle
    {
    public:
        Vector2 position;
        Size2 size;

        Rectangle()
        {
        }

        Rectangle(float aWidth, float aHeight):
            size(aWidth, aHeight)
        {
        }

        Rectangle(float aX, float aY, float aWidth, float aHeight):
            position(aX, aY), size(aWidth, aHeight)
        {
        }

        Rectangle(const Vector2& aPosition, float aWidth, float aHeight):
            position(aPosition), size(aWidth, aHeight)
        {
        }

        Rectangle(const Vector2& aPosition, const Size2& aSize):
            position(aPosition), size(aSize)
        {
        }

        Rectangle(const Rectangle& copy):
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

        bool containsPoint(float aX, float aY) const
        {
            return aX >= position.x && aX <= (position.x + size.width) &&
                aY >= position.y && aY <= (position.y + size.height);
        }

        bool containsPoint(const Vector2& point) const
        {
            return point.x >= position.x && point.x <= (position.x + size.width) &&
                point.y >= position.y && point.y <= (position.y + size.height);
        }

        bool contains(float aX, float aY, float aWidth, float aHeight) const
        {
            return containsPoint(aX, aY) && containsPoint(aX + aWidth, aY + aHeight);
        }

        bool contains(const Rectangle& r) const
        {
            return contains(r.position.x, r.position.y, r.size.width, r.size.height);
        }

        bool intersects(float aX, float aY, float aWidth, float aHeight) const
        {
            float t;
            if ((t = aX - position.x) > size.width || -t > aWidth)
                return false;
            if ((t = aY - position.y) > size.height || -t > aHeight)
                return false;
            return true;
        }

        bool intersects(const Rectangle& r) const
        {
            return intersects(r.position.x, r.position.y, r.size.width, r.size.height);
        }

        static bool intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        void inflate(float horizontalAmount, float verticalAmount)
        {
            position.x -= horizontalAmount;
            position.y -= verticalAmount;
            size.width += horizontalAmount * 2;
            size.height += verticalAmount * 2;
        }

        Rectangle& operator=(const Rectangle& r)
        {
            position.x = r.position.x;
            position.y = r.position.y;
            size.width = r.size.width;
            size.height = r.size.height;
            return *this;
        }

        bool operator==(const Rectangle& r) const
        {
            return position.x == r.position.x && size.width == r.size.width &&
                position.y == r.position.y && size.height == r.size.height;
        }

        bool operator!=(const Rectangle& r) const
        {
            return position.x != r.position.x || size.width != r.size.width ||
                position.y != r.position.y || size.height != r.size.height;
        }

        inline Rectangle operator*(float scalar) const
        {
            return Rectangle(position.x * scalar, position.y * scalar,
                             size.width * scalar, size.height * scalar);
        }

        inline Rectangle& operator*=(float scalar)
        {
            position.x *= scalar;
            position.y *= scalar;
            size.width *= scalar;
            size.height *= scalar;
            return *this;
        }

        inline Rectangle operator/(float scalar) const
        {
            return Rectangle(position.x / scalar, position.y / scalar,
                             size.width / scalar, size.height / scalar);
        }

        inline Rectangle& operator/=(float scalar)
        {
            position.x /= scalar;
            position.y /= scalar;
            size.width /= scalar;
            size.height /= scalar;
            return *this;
        }
    };
}
