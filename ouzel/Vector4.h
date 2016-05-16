// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "CompileConfig.h"
#if defined(OUZEL_SUPPORTS_SSE)
#include <xmmintrin.h>
#endif

namespace ouzel
{
    /**
     * Defines 4-element floating point vector.
     */
    class Vector4
    {
    public:
        static Vector4 ZERO;
        static Vector4 ONE;
        static Vector4 UNIT_X;
        static Vector4 UNIT_Y;
        static Vector4 UNIT_Z;
        static Vector4 UNIT_W;

#if defined(OUZEL_SUPPORTS_SSE)
        union
        {
            __m128 v;
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };
        };
#else
        float x;
        float y;
        float z;
        float w;
#endif

        /**
         * Constructs a new vector initialized to all zeros.
         */
        Vector4();

        /**
         * Constructs a new vector initialized to the specified values.
         *
         * @param pX The x coordinate.
         * @param pY The y coordinate.
         * @param pZ The z coordinate.
         * @param pW The w coordinate.
         */
        Vector4(float pX, float pY, float pZ, float pW);

        /**
         * Constructs a new vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y, z, w.
         */
        Vector4(const float* array);

        /**
         * Constructs a vector that describes the direction between the specified points.
         *
         * @param p1 The first point.
         * @param p2 The second point.
         */
        Vector4(const Vector4& p1, const Vector4& p2);

        /**
         * Constructor.
         *
         * Creates a new vector that is a copy of the specified vector.
         *
         * @param copy The vector to copy.
         */
        Vector4(const Vector4& copy);

        /**
         * Creates a new vector from an integer interpreted as an RGBA value.
         * E.g. 0xff0000ff represents opaque red or the vector (1, 0, 0, 1).
         *
         * @param color The integer to interpret as an RGBA value.
         *
         * @return A vector corresponding to the interpreted RGBA color.
         */
        static Vector4 fromColor(unsigned int color);

        /**
         * Destructor.
         */
        ~Vector4();

        /**
         * Indicates whether this vector contains all zeros.
         *
         * @return true if this vector contains all zeros, false otherwise.
         */
        bool isZero() const;

        /**
         * Indicates whether this vector contains all ones.
         *
         * @return true if this vector contains all ones, false otherwise.
         */
        bool isOne() const;

        /**
         * Returns the angle (in radians) between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The angle between the two vectors (in radians).
         */
        static float angle(const Vector4& v1, const Vector4& v2);

        /**
         * Adds the elements of the specified vector to this one.
         *
         * @param v The vector to add.
         */
        void add(const Vector4& v);

        /**
         * Adds the specified vectors and stores the result in dst.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst A vector to store the result in.
         */
        static void add(const Vector4& v1, const Vector4& v2, Vector4& dst);

        /**
         * Clamps this vector within the specified range.
         *
         * @param min The minimum value.
         * @param max The maximum value.
         */
        void clamp(const Vector4& min, const Vector4& max);

        /**
         * Clamps the specified vector within the specified range and returns it in dst.
         *
         * @param v The vector to clamp.
         * @param min The minimum value.
         * @param max The maximum value.
         * @param dst A vector to store the result in.
         */
        static void clamp(const Vector4& v, const Vector4& min, const Vector4& max, Vector4& dst);

        /**
         * Returns the distance between this vector and v.
         *
         * @param v The other vector.
         *
         * @return The distance between this vector and v.
         *
         * @see distanceSquared
         */
        float distance(const Vector4& v) const;

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
        float distanceSquared(const Vector4& v) const;

        /**
         * Returns the dot product of this vector and the specified vector.
         *
         * @param v The vector to compute the dot product with.
         *
         * @return The dot product.
         */
        float dot(const Vector4& v) const;

        /**
         * Returns the dot product between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The dot product between the vectors.
         */
        static float dot(const Vector4& v1, const Vector4& v2);

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
        float lengthSquared() const;

        /**
         * Negates this vector.
         */
        void negate();

        /**
         * Normalizes this vector.
         *
         * This method normalizes this Vector4 so that it is of
         * unit length (in other words, the length of the vector
         * after calling this method will be 1.0f). If the vector
         * already has unit length or if the length of the vector
         * is zero, this method does nothing.
         *
         * @return This vector, after the normalization occurs.
         */
        Vector4& normalize();

        /**
         * Normalizes this vector and stores the result in dst.
         *
         * If the vector already has unit length or if the length
         * of the vector is zero, this method simply copies the
         * current vector into dst.
         *
         * @param dst The destination vector.
         */
        void normalize(Vector4& dst) const;

        /**
         * Scales all elements of this vector by the specified value.
         *
         * @param scalar The scalar value.
         */
        void scale(float scalar);

        /**
         * Sets the elements of this vector to the specified values.
         *
         * @param pX The new x coordinate.
         * @param pY The new y coordinate.
         * @param pZ The new z coordinate.
         * @param pW The new w coordinate.
         */
        void set(float pX, float pY, float pZ, float pW);

        /**
         * Sets the elements of this vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y, z, w.
         */
        void set(const float* array);

        /**
         * Sets the elements of this vector to those in the specified vector.
         *
         * @param v The vector to copy.
         */
        void set(const Vector4& v);

        /**
         * Sets this vector to the directional vector between the specified points.
         *
         * @param p1 The first point.
         * @param p2 The second point.
         */
        void set(const Vector4& p1, const Vector4& p2);

        /**
         * Subtracts this vector and the specified vector as (this - v)
         * and stores the result in this vector.
         *
         * @param v The vector to subtract.
         */
        void subtract(const Vector4& v);

        /**
         * Subtracts the specified vectors and stores the result in dst.
         * The resulting vector is computed as (v1 - v2).
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst The destination vector.
         */
        static void subtract(const Vector4& v1, const Vector4& v2, Vector4& dst);

        /**
         * Calculates the sum of this vector with the given vector.
         *
         * Note: this does not modify this vector.
         *
         * @param v The vector to add.
         * @return The vector sum.
         */
        inline const Vector4 operator+(const Vector4& v) const
        {
            Vector4 result(*this);
            result.add(v);
            return result;
        }

        /**
         * Adds the given vector to this vector.
         *
         * @param v The vector to add.
         * @return This vector, after the addition occurs.
         */
        inline Vector4& operator+=(const Vector4& v)
        {
            add(v);
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
        inline const Vector4 operator-(const Vector4& v) const
        {
            Vector4 result(*this);
            result.subtract(v);
            return result;
        }

        /**
         * Subtracts the given vector from this vector.
         *
         * @param v The vector to subtract.
         * @return This vector, after the subtraction occurs.
         */
        inline Vector4& operator-=(const Vector4& v)
        {
            subtract(v);
            return *this;
        }

        /**
         * Calculates the negation of this vector.
         *
         * Note: this does not modify this vector.
         *
         * @return The negation of this vector.
         */
        inline const Vector4 operator-() const
        {
            Vector4 result(*this);
            result.negate();
            return result;
        }

        /**
         * Calculates the scalar product of this vector with the given value.
         *
         * Note: this does not modify this vector.
         *
         * @param scalar The value to scale by.
         * @return The scaled vector.
         */
        inline const Vector4 operator*(float scalar) const
        {
            Vector4 result(*this);
            result.scale(scalar);
            return result;
        }

        /**
         * Scales this vector by the given value.
         *
         * @param scalar The value to scale by.
         * @return This vector, after the scale occurs.
         */
        inline Vector4& operator*=(float scalar)
        {
            scale(scalar);
            return *this;
        }

        /**
         * Returns the components of this vector divided by the given constant
         *
         * Note: this does not modify this vector.
         *
         * @param d the constant to divide this vector with
         * @return a smaller vector
         */
        inline const Vector4 operator/(float scalar) const
        {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        inline Vector4& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        /**
         * Determines if this vector is less than the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is less than the given vector, false otherwise.
         */
        inline bool operator<(const Vector4& v) const
        {
            if (x == v.x)
            {
                if (y == v.y)
                {
                    if (z == v.z)
                    {
                        return w < v.w;
                    }
                    return z < v.z;
                }
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
        inline bool operator==(const Vector4& v) const
        {
            return x==v.x && y==v.y && z==v.z && w==v.w;
        }

        /**
         * Determines if this vector is not equal to the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is not equal to the given vector, false otherwise.
         */
        inline bool operator!=(const Vector4& v) const
        {
            return x!=v.x || y!=v.y || z!=v.z || w!=v.w;
        }
    };

    /**
     * Calculates the scalar product of the given vector with the given value.
     *
     * @param x The value to scale by.
     * @param v The vector to scale.
     * @return The scaled vector.
     */
    inline const Vector4 operator*(float x, const Vector4& v)
    {
        Vector4 result(v);
        result.scale(x);
        return result;
    }
}
