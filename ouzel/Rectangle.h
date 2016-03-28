// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"
#include "Size2.h"

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
    Rectangle();

    /**
     * Constructs a new rectangle with the x = 0, y = 0 and the specified width and height.
     *
     * @param pWidth The width of the rectangle.
     * @param pHeight The height of the rectangle.
     */
    Rectangle(float pWidth, float pHeight);

    /**
     * Constructs a new rectangle with the specified x, y, width and height.
     *
     * @param pX The x-coordinate of the rectangle.
     * @param pY The y-coordinate of the rectangle.
     * @param pWidth The width of the rectangle.
     * @param pHeight The height of the rectangle.
     */
    Rectangle(float pX, float pY, float pWidth, float pHeight);

    Rectangle(const Vector2& position, float pWidth, float pHeight);

    Rectangle(const Vector2& position, const Size2& size);

    /**
     * Constructs a new rectangle that is a copy of the specified rectangle.
     *
     * @param copy The rectangle to copy.
     */
    Rectangle(const Rectangle& copy);

    /**
     * Destructor.
     */
    ~Rectangle();

    /**
     * Gets a value that indicates whether the rectangle is empty.
     *
     * @return true if the rectangle is empty, false otherwise.
     */
    bool isEmpty() const;

    /**
     * Sets the values of this rectangle to the specified values.
     *
     * @param pX The x-coordinate of the rectangle.
     * @param pY The y-coordinate of the rectangle.
     * @param pWidth The width of the rectangle.
     * @param pHeight The height of the rectangle.
     */
    void set(float pX, float pY, float pWidth, float pHeight);

    void set(const Vector2& position, float pWidth, float pHeight);

    /**
     * Sets the values of this rectangle to those in the specified rectangle.
     *
     * @param r The rectangle to copy.
     */
    void set(const Rectangle& r);

    /**
     * Sets the x-coordinate and y-coordinate values of this rectangle to the specified values.
     *
     * @param pX The x-coordinate of the rectangle.
     * @param pY The y-coordinate of the rectangle.
     */
    void setPosition(float pX, float pY);

    void setPosition(const Vector2 position);

    /**
     * Returns the x-coordinate of the left side of the rectangle.
     *
     * @return The x-coordinate of the left side of the rectangle.
     */
    float left() const;

    /**
     * Returns the y-coordinate of the top of the rectangle.
     *
     * @return The y-coordinate of the top of the rectangle.
     */
    float top() const;

    /**
     * Returns the x-coordinate of the right side of the rectangle.
     *
     * @return The x-coordinate of the right side of the rectangle.
     */
    float right() const;

    /**
     * Returns the y-coordinate of the bottom of the rectangle.
     *
     * @return The y-coordinate of the bottom of the rectangle.
     */
    float bottom() const;

    /**
     * Determines whether this rectangle contains a specified point.
     *
     * @param pX The x-coordinate of the point.
     * @param pY The y-coordinate of the point.
     *
     * @return true if the rectangle contains the point, false otherwise.
     */
    bool containsPoint(float pX, float pY) const;

    bool containsPoint(const Vector2& point) const;

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param pX The x-coordinate of the rectangle.
     * @param pY The y-coordinate of the rectangle.
     * @param pWidth The width of the rectangle.
     * @param pHeight The height of the rectangle.
     *
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(float pX, float pY, float pWidth, float pHeight) const;

    /**
     * Determines whether this rectangle contains a specified rectangle.
     *
     * @param r The rectangle.
     *
     * @return true if the rectangle contains the specified rectangle, false
     * otherwise.
     */
    bool contains(const Rectangle& r) const;

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     * Rectangles intersect if there is a common point that is contained in both rectangles.
     *
     * @param pX The x-coordinate of the rectangle.
     * @param pY The y-coordinate of the rectangle.
     * @param pWidth The width of the rectangle.
     * @param pHeight The height of the rectangle.
     *
     * @return true if the specified Rectangle intersects with this one, false otherwise.
     */
    bool intersects(float pX, float pY, float pWidth, float pHeight) const;

    /**
     * Determines whether a specified rectangle intersects with this rectangle.
     *
     * @param r The rectangle.
     *
     * @return true if the specified rectangle intersects with this one, false
     * otherwise.
     */
    bool intersects(const Rectangle& r) const;

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
    void inflate(float horizontalAmount, float verticalAmount);

    /**
     * operator =
     */
    Rectangle& operator=(const Rectangle& r);

    /**
     * operator ==
     */
    bool operator==(const Rectangle& r) const;

    /**
     * operator !=
     */
    bool operator!=(const Rectangle& r) const;
};

}
