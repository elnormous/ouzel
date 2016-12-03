// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Vector2.h"
#include "math/Size2.h"

namespace ouzel
{
    class Rectangle
    {
    public:
        float x;
        float y;
        float width;
        float height;

        Rectangle():
            x(0.0f), y(0.0f), width(0.0f), height(0.0f)
        {
        }

        Rectangle(float aWidth, float aHeight):
            x(0.0f), y(0.0f), width(aWidth), height(aHeight)
        {
        }

        Rectangle(float aX, float aY, float aWidth, float aHeight):
            x(aX), y(aY), width(aWidth), height(aHeight)
        {
        }

        Rectangle(const Vector2& position, float aWidth, float aHeight):
            x(position.x), y(position.y), width(aWidth), height(aHeight)
        {
        }

        Rectangle(const Vector2& position, const Size2& size):
            x(position.x), y(position.y), width(size.width), height(size.height)
        {
        }

        Rectangle(const Rectangle& copy):
            x(copy.x), y(copy.y), width(copy.width), height(copy.height)
        {
        }

        inline bool isEmpty() const
        {
            return width == 0 && height == 0;
        }

        void set(float newX, float newY, float newWidth, float newHeight)
        {
            x = newX;
            y = newY;
            width = newWidth;
            height = newHeight;
        }

        void set(const Vector2& position, float newWidth, float newHeight)
        {
            x = position.x;
            y = position.y;
            width = newWidth;
            height = newHeight;
        }

        void setPosition(float newX, float newY)
        {
            x = newX;
            y = newY;
        }

        void setPosition(const Vector2& position)
        {
            x = position.x;
            y = position.y;
        }

        float left() const
        {
            return x;
        }

        float bottom() const
        {
            return y;
        }

        float right() const
        {
            return x + width;
        }

        float top() const
        {
            return y + height;
        }

        Vector2 bottomLeft() const
        {
            return Vector2(x, y);
        }

        Vector2 topRight() const
        {
            return Vector2(x + width, y + height);
        }

        bool containsPoint(float aX, float aY) const
        {
            return aX >= x && aX <= (x + width) && aY >= y && aY <= (y + height);
        }

        bool containsPoint(const Vector2& point) const
        {
            return point.x >= x && point.x <= (x + width) && point.y >= y && point.y <= (y + height);
        }

        bool contains(float aX, float aY, float aWidth, float aHeight) const
        {
            return containsPoint(aX, aY) && containsPoint(aX + aWidth, aY + aHeight);
        }

        bool contains(const Rectangle& r) const
        {
            return contains(r.x, r.y, r.width, r.height);
        }

        bool intersects(float aX, float aY, float aWidth, float aHeight) const
        {
            float t;
            if ((t = aX - x) > width || -t > aWidth)
                return false;
            if ((t = aY - y) > height || -t > aHeight)
                return false;
            return true;
        }

        bool intersects(const Rectangle& r) const
        {
            return intersects(r.x, r.y, r.width, r.height);
        }

        static bool intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

        void inflate(float horizontalAmount, float verticalAmount)
        {
            x -= horizontalAmount;
            y -= verticalAmount;
            width += horizontalAmount * 2;
            height += verticalAmount * 2;
        }

        Rectangle& operator=(const Rectangle& r)
        {
            x = r.x;
            y = r.y;
            width = r.width;
            height = r.height;
            return *this;
        }

        bool operator==(const Rectangle& r) const
        {
            return x == r.x && width == r.width && y == r.y && height == r.height;
        }

        bool operator!=(const Rectangle& r) const
        {
            return x != r.x || width != r.width || y != r.y || height != r.height;
        }

        inline Rectangle operator*(float scalar) const
        {
            return Rectangle(x * scalar, y * scalar, width * scalar, height * scalar);
        }

        inline Rectangle& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            width *= scalar;
            height *= scalar;
            return *this;
        }

        inline Rectangle operator/(float scalar) const
        {
            return Rectangle(x / scalar, y / scalar, width / scalar, height / scalar);
        }

        inline Rectangle& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            width /= scalar;
            height /= scalar;
            return *this;
        }
    };
}
