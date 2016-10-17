// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Vector2.h"
#include "math/Size2.h"

namespace ouzel
{

/**
 * Defines a rectangle.
 */
class Rectangle
{
public:

    /**
     * Specifies the x-coordinate of the rectangle.
     */
    float x;

    /**
     * Specifies the y-coordinate of the rectangle.
     */
    float y;

    /**
     * Specifies the width of the rectangle.
     */
    float width;

    /**
     * Specifies the height of the rectangle.
     */
    float height;

    /**
     * Constructs a new rectangle initialized to all zeros.
     */
    Rectangle():
        x(0.0f), y(0.0f), width(0.0f), height(0.0f)
    {
    }

    /**
     * Constructs a new rectangle with the x = 0, y = 0 and the specified width and height.
     *
     * @param aWidth The width of the rectangle.
     * @param aHeight The height of the rectangle.
     */
    Rectangle(float aWidth, float aHeight):
        x(0.0f), y(0.0f), width(aWidth), height(aHeight)
    {
    }

    /**
     * Constructs a new rectangle with the specified x, y, width and height.
     *
     * @param aX The x-coordinate of the rectangle.
     * @param aY The y-coordinate of the rectangle.
     * @param aWidth The width of the rectangle.
     * @param aHeight The height of the rectangle.
     */
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

    /**
     * Constructs a new rectangle that is a copy of the specified rectangle.
     *
     * @param copy The rectangle to copy.
     */
    Rectangle(const Rectangle& copy)
    {
        set(copy);
    }

    /**
     * Gets a value that indicates whether the rectangle is empty.
     *
     * @return true if the rectangle is empty, false otherwise.
     */
    inline bool isEmpty() const
    {
        return (width == 0 && height == 0);
    }

    /**
     * Sets the values of this rectangle to the specified values.
     *
     * @param newX The x-coordinate of the rectangle.
     * @param newY The y-coordinate of the rectangle.
     * @param newWidth The width of the rectangle.
     * @param newHeight The height of the rectangle.
     */
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

    /**
     * Sets the values of this rectangle to those in the specified rectangle.
     *
     * @param r The rectangle to copy.
     */
    void set(const Rectangle& r)
    {
        set(r.x, r.y, r.width, r.height);
    }

    /**
     * Sets the x-coordinate and y-coordinate values of this rectangle to the specified values.
     *
     * @param newX The x-coordinate of the rectangle.
     * @param newY The y-coordinate of the rectangle.
     */
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

    /**
     * Returns the x-coordinate of the left side of the rectangle.
     *
     * @return The x-coordinate of the left side of the rectangle.
     */
    float left() const
    {
        return x;
    }

    /**
     * Returns the y-coordinate of the bottom of the rectangle.
     *
     * @return The y-coordinate of the bottom of the rectangle.
     */
    float bottom() const
    {
        return y;
    }

    /**
     * Returns the x-coordinate of the right side of the rectangle.
     *
     * @return The x-coordinate of the right side of the rectangle.
     */
    float right() const
    {
        return x + width;
    }

    /**
     * Returns the y-coordinate of the top of the rectangle.
     *
     * @return The y-coordinate of the top of the rectangle.
     */
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

    /**
     * Determines whether this rectangle contains a specified point.
     *
     * @param aX The x-coordinate of the point.
     * @param aY The y-coordinate of the point.
     *
     * @return true if the rectangle contains the point, false otherwise.
     */
    bool containsPoint(float aX, float aY) const
    {
        return (aX >= x && aX <= (x + width) && aY >= y && aY <= (y + height));
    }

    bool containsPoint(const Vector2& point) const
    {
        return (point.x >= x && point.x <= (x + width) && point.y >= y && point.y <= (y + height));
    }

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param aX The x-coordinate of the rectangle.
     * @param aY The y-coordinate of the rectangle.
     * @param aWidth The width of the rectangle.
     * @param aHeight The height of the rectangle.
     *
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(float aX, float aY, float aWidth, float aHeight) const
    {
        return (containsPoint(aX, aY) && containsPoint(aX + aWidth, aY + aHeight));
    }

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param r The rectangle.
     *
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(const Rectangle& r) const
    {
        return contains(r.x, r.y, r.width, r.height);
    }

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     * Rectangles intersect if there is a common point that is contained in both rectangles.
     *
     * @param aX The x-coordinate of the rectangle.
     * @param aY The y-coordinate of the rectangle.
     * @param aWidth The width of the rectangle.
     * @param aHeight The height of the rectangle.
     *
     * @return true if the specified Rectangle intersects with this one, false otherwise.
     */
    bool intersects(float aX, float aY, float aWidth, float aHeight) const
    {
        float t;
        if ((t = aX - x) > width || -t > aWidth)
            return false;
        if ((t = aY - y) > height || -t > aHeight)
            return false;
        return true;
    }

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     *
     * @param r The rectangle.
     *
     * @return true if the specified rectangle intersects with this one, false
     * otherwise.
     */
    bool intersects(const Rectangle& r) const
    {
        return intersects(r.x, r.y, r.width, r.height);
    }

    /**
     * Computes the intersection of two rectangles and returns the result.
     *
     * @param r1 The first rectangle.
     * @param r2 The second rectangle.
     * @param dst Populated with the resulting intersection, or Rectangle.empty if they do not intersect.
     *
     * @return True if the two rectangles intersect, false otherwise.
     */
    static bool intersect(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

    /**
     * Returns a new rectangle that exactly contains two other rectangles.
     *
     * @param r1 The first rectangle to contain.
     * @param r2 The second rectangle to contain.
     * @param dst A rectangle to store the union of the two rectangle parameters.
     */
    static void combine(const Rectangle& r1, const Rectangle& r2, Rectangle* dst);

    /**
     * Pushes the edges of the Rectangle out by the horizontal and vertical values specified.
     *
     * Each corner of the Rectangle is pushed away from the center of the rectangle
     * by the specified amounts. This results in the width and height of the Rectangle
     * increasing by twice the values provided.
     *
     * @param horizontalAmount The value to push the sides out by.
     * @param verticalAmount The value to push the top and bottom out by.
     */
    void inflate(float horizontalAmount, float verticalAmount)
    {
        x -= horizontalAmount;
        y -= verticalAmount;
        width += horizontalAmount * 2;
        height += verticalAmount * 2;
    }

    /**
     * operator =
     */
    Rectangle& operator=(const Rectangle& r)
    {
        x = r.x;
        y = r.y;
        width = r.width;
        height = r.height;
        return *this;
    }

    /**
     * operator ==
     */
    bool operator==(const Rectangle& r) const
    {
        return (x == r.x && width == r.width && y == r.y && height == r.height);
    }

    /**
     * operator !=
     */
    bool operator!=(const Rectangle& r) const
    {
        return (x != r.x || width != r.width || y != r.y || height != r.height);
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
