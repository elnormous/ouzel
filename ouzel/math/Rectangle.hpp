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
            position.v[0] = newX;
            position.v[1] = newY;
            size.v[0] = newWidth;
            size.v[1] = newHeight;
        }

        void set(const Vector2& newPosition, float newWidth, float newHeight)
        {
            position = newPosition;
            size.v[0] = newWidth;
            size.v[1] = newHeight;
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
            return position.v[0] + size.v[0];
        }

        float top() const
        {
            return position.v[1] + size.v[1];
        }

        Vector2 bottomLeft() const
        {
            return position;
        }

        Vector2 topRight() const
        {
            return Vector2(position.v[0] + size.v[0], position.v[1] + size.v[1]);
        }

        bool containsPoint(float aX, float aY) const
        {
            return aX >= position.v[0] && aX <= (position.v[0] + size.v[0]) &&
                aY >= position.v[1] && aY <= (position.v[1] + size.v[1]);
        }

        bool containsPoint(const Vector2& point) const
        {
            return point.v[0] >= position.v[0] && point.v[0] <= (position.v[0] + size.v[0]) &&
                point.v[1] >= position.v[1] && point.v[1] <= (position.v[1] + size.v[1]);
        }

        bool contains(float aX, float aY, float aWidth, float aHeight) const
        {
            return containsPoint(aX, aY) && containsPoint(aX + aWidth, aY + aHeight);
        }

        bool contains(const Rectangle& r) const
        {
            return contains(r.position.v[0], r.position.v[1], r.size.v[0], r.size.v[1]);
        }

        bool intersects(float aX, float aY, float aWidth, float aHeight) const
        {
            float t;
            if ((t = aX - position.v[0]) > size.v[0] || -t > aWidth)
                return false;
            if ((t = aY - position.v[1]) > size.v[1] || -t > aHeight)
                return false;
            return true;
        }

        bool intersects(const Rectangle& r) const
        {
            return intersects(r.position.v[0], r.position.v[1], r.size.v[0], r.size.v[1]);
        }

        static bool intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        void inflate(float horizontalAmount, float verticalAmount)
        {
            position.v[0] -= horizontalAmount;
            position.v[1] -= verticalAmount;
            size.v[0] += horizontalAmount * 2;
            size.v[1] += verticalAmount * 2;
        }

        Rectangle& operator=(const Rectangle& r)
        {
            position.v[0] = r.position.v[0];
            position.v[1] = r.position.v[1];
            size.v[0] = r.size.v[0];
            size.v[1] = r.size.v[1];
            return *this;
        }

        bool operator==(const Rectangle& r) const
        {
            return position.v[0] == r.position.v[0] && size.v[0] == r.size.v[0] &&
                position.v[1] == r.position.v[1] && size.v[1] == r.size.v[1];
        }

        bool operator!=(const Rectangle& r) const
        {
            return position.v[0] != r.position.v[0] || size.v[0] != r.size.v[0] ||
                position.v[1] != r.position.v[1] || size.v[1] != r.size.v[1];
        }

        inline Rectangle operator*(float scalar) const
        {
            return Rectangle(position.v[0] * scalar, position.v[1] * scalar,
                             size.v[0] * scalar, size.v[1] * scalar);
        }

        inline Rectangle& operator*=(float scalar)
        {
            position.v[0] *= scalar;
            position.v[1] *= scalar;
            size.v[0] *= scalar;
            size.v[1] *= scalar;
            return *this;
        }

        inline Rectangle operator/(float scalar) const
        {
            return Rectangle(position.v[0] / scalar, position.v[1] / scalar,
                             size.v[0] / scalar, size.v[1] / scalar);
        }

        inline Rectangle& operator/=(float scalar)
        {
            position.v[0] /= scalar;
            position.v[1] /= scalar;
            size.v[0] /= scalar;
            size.v[1] /= scalar;
            return *this;
        }
    };
}
