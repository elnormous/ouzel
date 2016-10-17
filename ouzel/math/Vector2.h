// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cmath>

namespace ouzel
{
    class Vector3;
    class Vector4;

    /**
     * Defines a 2-element floating point vector.
     */
    class Vector2
    {
    public:
        static const Vector2 ZERO;
        static const Vector2 ONE;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;

        float x;
        float y;

        /**
         * Constructs a new vector initialized to all zeros.
         */
        Vector2():
            x(0.0f), y(0.0f)
        {
        }

        /**
         * Constructs a new vector initialized to the specified values.
         *
         * @param aX The x coordinate.
         * @param aY The y coordinate.
         */
        Vector2(float aX, float aY):
            x(aX), y(aY)
        {
        }

        /**
         * Constructs a new vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y.
         */
        Vector2(const float* array);

        /**
         * Constructs a vector that describes the direction between the specified points.
         *
         * @param p1 The first point.
         * @param p2 The second point.
         */
        Vector2(const Vector2& p1, const Vector2& p2);

        /**
         * Constructs a new vector that is a copy of the specified vector.
         *
         * @param copy The vector to copy.
         */
        Vector2(const Vector2& copy):
            x(copy.x),
            y(copy.y)
        {
        }

        Vector2(const Vector3& v);

        Vector2& operator=(const Vector3& v);

        Vector2(const Vector4& v);

        Vector2& operator=(const Vector4& v);

        /**
         * Indicates whether this vector contains all zeros.
         *
         * @return true if this vector contains all zeros, false otherwise.
         */
        inline bool isZero() const
        {
            return x == 0.0f && y == 0.0f;
        }

        /**
         * Indicates whether this vector contains all ones.
         *
         * @return true if this vector contains all ones, false otherwise.
         */
        inline bool isOne() const
        {
            return x == 1.0f && y == 1.0f;
        }

        /**
         * Returns the angle (in radians) between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The angle between the two vectors (in radians).
         */
        static float angle(const Vector2& v1, const Vector2& v2);

        /**
         * Adds the elements of the specified vector to this one.
         *
         * @param v The vector to add.
         */
        void add(const Vector2& v)
        {
            x += v.x;
            y += v.y;
        }

        /**
         * Adds the specified vectors and stores the result in dst.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst A vector to store the result in.
         */
        static void add(const Vector2& v1, const Vector2& v2, Vector2& dst);

        /**
         * Clamps this vector within the specified range.
         *
         * @param min The minimum value.
         * @param max The maximum value.
         */
        void clamp(const Vector2& min, const Vector2& max);

        /**
         * Clamps the specified vector within the specified range and returns it in dst.
         *
         * @param v The vector to clamp.
         * @param min The minimum value.
         * @param max The maximum value.
         * @param dst A vector to store the result in.
         */
        static void clamp(const Vector2& v, const Vector2& min, const Vector2& max, Vector2& dst);

        /**
         * Returns the distance between this vector and v.
         *
         * @param v The other vector.
         *
         * @return The distance between this vector and v.
         *
         * @see distanceSquared
         */
        float distance(const Vector2& v) const;

        /**
         * Returns the squared distance between this vector and v.
         *
         * When it is not necessary to get the exact distance between
         * two vectors (for example, when simply comparing the
         * distance between different vectors), it is advised to use
         * this method instead of distance.
         *
         * @param v The other vector.
         *
         * @return The squared distance between this vector and v.
         *
         * @see distance
         */
        float distanceSquared(const Vector2& v) const
        {
            float dx = v.x - x;
            float dy = v.y - y;
            return (dx * dx + dy * dy);
        }

        /**
         * Returns the dot product of this vector and the specified vector.
         *
         * @param v The vector to compute the dot product with.
         *
         * @return The dot product.
         */
        float dot(const Vector2& v) const
        {
            return (x * v.x + y * v.y);
        }

        /**
         * Returns the dot product between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The dot product between the vectors.
         */
        static float dot(const Vector2& v1, const Vector2& v2)
        {
            return (v1.x * v2.x + v1.y * v2.y);
        }

        /**
         * Computes the length of this vector.
         *
         * @return The length of the vector.
         *
         * @see lengthSquared
         */
        float length() const;

        /**
         * Returns the squared length of this vector.
         *
         * When it is not necessary to get the exact length of a
         * vector (for example, when simply comparing the lengths of
         * different vectors), it is advised to use this method
         * instead of length.
         *
         * @return The squared length of the vector.
         *
         * @see length
         */
        float lengthSquared() const
        {
            return (x * x + y * y);
        }

        /**
         * Negates this vector.
         */
        void negate()
        {
            x = -x;
            y = -y;
        }

        /**
         * Normalizes this vector.
         *
         * This method normalizes this Vector2 so that it is of
         * unit length (in other words, the length of the vector
         * after calling this method will be 1.0f). If the vector
         * already has unit length or if the length of the vector
         * is zero, this method does nothing.
         *
         * @return This vector, after the normalization occurs.
         */
        Vector2& normalize();

        /**
         * Normalizes this vector and stores the result in dst.
         *
         * If the vector already has unit length or if the length
         * of the vector is zero, this method simply copies the
         * current vector into dst.
         *
         * @param dst The destination vector.
         */
        void normalize(Vector2& dst) const;

        /**
         * Scales all elements of this vector by the specified value.
         *
         * @param scalar The scalar value.
         */
        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
        }

        /**
         * Scales each element of this vector by the matching component of scale.
         *
         * @param scale The vector to scale by.
         */
        void scale(const Vector2& scale)
        {
            x *= scale.x;
            y *= scale.y;
        }

        /**
         * Rotates this vector by angle (specified in radians) around the 0,0 point.
         *
         * @param angle The angle to rotate by (in radians).
         */
        void rotate(float angle);

        /**
         * Rotates this vector by angle (specified in radians) around the given point.
         *
         * @param point The point to rotate around.
         * @param angle The angle to rotate by (in radians).
         */
        void rotate(const Vector2& point, float angle);

        /**
         * Sets the elements of this vector to the specified values.
         *
         * @param newX The new x coordinate.
         * @param newY The new y coordinate.
         */
        void set(float newX, float newY)
        {
            x = newX;
            y = newY;
        }

        /**
         * Sets the elements of this vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y.
         */
        void set(const float* array);

        /**
         * Sets the elements of this vector to those in the specified vector.
         *
         * @param v The vector to copy.
         */
        void set(const Vector2& v)
        {
            x = v.x;
            y = v.y;
        }

        /**
         * Sets this vector to the directional vector between the specified points.
         *
         * @param p1 The first point.
         * @param p2 The second point.
         */
        void set(const Vector2& p1, const Vector2& p2)
        {
            x = p2.x - p1.x;
            y = p2.y - p1.y;
        }

        /**
         * Subtracts this vector and the specified vector as (this - v)
         * and stores the result in this vector.
         *
         * @param v The vector to subtract.
         */
        void subtract(const Vector2& v)
        {
            x -= v.x;
            y -= v.y;
        }

        /**
         * Subtracts the specified vectors and stores the result in dst.
         * The resulting vector is computed as (v1 - v2).
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst The destination vector.
         */
        static void subtract(const Vector2& v1, const Vector2& v2, Vector2& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
        }

        /**
         * Updates this vector towards the given target using a smoothing function.
         * The given response time determines the amount of smoothing (lag). A longer
         * response time yields a smoother result and more lag. To force this vector to
         * follow the target closely, provide a response time that is very small relative
         * to the given elapsed time.
         *
         * @param target target value.
         * @param elapsedTime elapsed time between calls.
         * @param responseTime response time (in the same units as elapsedTime).
         */
        void smooth(const Vector2& target, float elapsedTime, float responseTime);

        float getMin() const;

        float getMax() const;

        /**
         * Calculates the sum of this vector with the given vector.
         *
         * Note: this does not modify this vector.
         *
         * @param v The vector to add.
         * @return The vector sum.
         */
        inline Vector2 operator+(const Vector2& v) const
        {
            return Vector2(x + v.x, y + v.y);
        }

        /**
         * Adds the given vector to this vector.
         *
         * @param v The vector to add.
         * @return This vector, after the addition occurs.
         */
        inline Vector2& operator+=(const Vector2& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        /**
         * Calculates the sum of this vector with the given vector.
         *
         * Note: this does not modify this vector.
         *
         * @param v The vector to add.
         * @return The vector sum.
         */
        inline Vector2 operator-(const Vector2& v) const
        {
            return Vector2(x - v.x, y - v.y);
        }

        /**
         * Subtracts the given vector from this vector.
         *
         * @param v The vector to subtract.
         * @return This vector, after the subtraction occurs.
         */
        inline Vector2& operator-=(const Vector2& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        /**
         * Calculates the negation of this vector.
         *
         * Note: this does not modify this vector.
         *
         * @return The negation of this vector.
         */
        inline Vector2 operator-() const
        {
            return Vector2(-x, -y);
        }

        /**
         * Calculates the scalar product of this vector with the given value.
         *
         * Note: this does not modify this vector.
         *
         * @param scalar The value to scale by.
         * @return The scaled vector.
         */
        inline Vector2 operator*(float scalar) const
        {
            return Vector2(x * scalar, y * scalar);
        }

        /**
         * Scales this vector by the given value.
         *
         * @param scalar The value to scale by.
         * @return This vector, after the scale occurs.
         */
        inline Vector2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        /**
         * Returns the components of this vector divided by the given constant
         *
         * Note: this does not modify this vector.
         *
         * @param scalar the constant to divide this vector with
         * @return a smaller vector
         */
        inline Vector2 operator/(float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
        }

        inline Vector2& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        /**
         * Determines if this vector is less than the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is less than the given vector, false otherwise.
         */
        inline bool operator<(const Vector2& v) const
        {
            if (x == v.x)
            {
                return y < v.y;
            }
            return x < v.x;
        }

        /**
         * Determines if this vector is equal to the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is equal to the given vector, false otherwise.
         */
        inline bool operator==(const Vector2& v) const
        {
            return x==v.x && y==v.y;
        }

        /**
         * Determines if this vector is not equal to the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is not equal to the given vector, false otherwise.
         */
        inline bool operator!=(const Vector2& v) const
        {
            return x!=v.x || y!=v.y;
        }

        /**
         * Get the angle in radians between this vector and the x axis
         *
         * @return Returns the angle
         */
        inline float getAngle() const
        {
            return atan2f(y, x);
        };
    };

    /**
     * Calculates the scalar product of the given vector with the given value.
     *
     * @param scalar The value to scale by.
     * @param v The vector to scale.
     * @return The scaled vector.
     */
    inline Vector2 operator*(float scalar, const Vector2& v)
    {
        return Vector2(v.x * scalar, v.y * scalar);
    }
}
