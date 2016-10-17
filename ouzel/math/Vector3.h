// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Matrix;
    class Vector2;
    class Vector4;

    /**
     * Defines a 3-element floating point vector.
     *
     * When using a vector to represent a surface normal,
     * the vector should typically be normalized.
     * Other uses of directional vectors may wish to leave
     * the magnitude of the vector intact. When used as a point,
     * the elements of the vector represent a position in 3D space.
     */
    class Vector3
    {
    public:
        static const Vector3 ZERO;
        static const Vector3 ONE;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;

        float x;
        float y;
        float z;

        /**
         * Constructs a new vector initialized to all zeros.
         */
        Vector3():
            x(0.0f), y(0.0f), z(0.0f)
        {
        }

        /**
         * Constructs a new vector initialized to the specified values.
         *
         * @param aX The x coordinate.
         * @param aY The y coordinate.
         * @param aZ The z coordinate.
         */
        Vector3(float aX, float aY, float aZ):
            x(aX), y(aY), z(aZ)
        {
        }

        /**
         * Constructs a new vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y, z.
         */
        Vector3(const float* array);

        /**
         * Constructs a vector that describes the direction between the specified points.
         *
         * @param p1 The first point.
         * @param p2 The second point.
         */
        Vector3(const Vector3& p1, const Vector3& p2);

        /**
         * Constructs a new vector that is a copy of the specified vector.
         *
         * @param copy The vector to copy.
         */
        Vector3(const Vector3& copy):
            x(copy.x),
            y(copy.y),
            z(copy.z)
        {
        }

        Vector3(const Vector2& v);

        Vector3& operator=(const Vector2& v);

        Vector3(const Vector4& v);

        Vector3& operator=(const Vector4& v);

        /**
         * Creates a new vector from an integer interpreted as an RGB value.
         * E.g. 0xff0000 represents red or the vector (1, 0, 0).
         *
         * @param color The integer to interpret as an RGB value.
         *
         * @return A vector corresponding to the interpreted RGB color.
         */
        static Vector3 fromColor(unsigned int color);

        /**
         * Indicates whether this vector contains all zeros.
         *
         * @return true if this vector contains all zeros, false otherwise.
         */
        inline bool isZero() const
        {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        /**
         * Indicates whether this vector contains all ones.
         *
         * @return true if this vector contains all ones, false otherwise.
         */
        inline bool isOne() const
        {
            return x == 1.0f && y == 1.0f && z == 1.0f;
        }

        /**
         * Returns the angle (in radians) between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The angle between the two vectors (in radians).
         */
        static float angle(const Vector3& v1, const Vector3& v2);

        /**
         * Adds the elements of the specified vector to this one.
         *
         * @param v The vector to add.
         */
        void add(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        /**
         * Adds the specified vectors and stores the result in dst.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst A vector to store the result in.
         */
        static void add(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = v1.x + v2.x;
            dst.y = v1.y + v2.y;
            dst.z = v1.z + v2.z;
        }

        /**
         * Clamps this vector within the specified range.
         *
         * @param min The minimum value.
         * @param max The maximum value.
         */
        void clamp(const Vector3& min, const Vector3& max);

        /**
         * Clamps the specified vector within the specified range and returns it in dst.
         *
         * @param v The vector to clamp.
         * @param min The minimum value.
         * @param max The maximum value.
         * @param dst A vector to store the result in.
         */
        static void clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3& dst);

        /**
         * Sets this vector to the cross product between itself and the specified vector.
         *
         * @param v The vector to compute the cross product with.
         */
        void cross(const Vector3& v);

        /**
         * Computes the cross product of the specified vectors and stores the result in dst.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst A vector to store the result in.
         */
        static void cross(const Vector3& v1, const Vector3& v2, Vector3& dst);

        /**
         * Returns the distance between this vector and v.
         *
         * @param v The other vector.
         *
         * @return The distance between this vector and v.
         *
         * @see distanceSquared
         */
        float distance(const Vector3& v) const;

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
        float distanceSquared(const Vector3& v) const
        {
            float dx = v.x - x;
            float dy = v.y - y;
            float dz = v.z - z;

            return (dx * dx + dy * dy + dz * dz);
        }

        /**
         * Returns the dot product of this vector and the specified vector.
         *
         * @param v The vector to compute the dot product with.
         *
         * @return The dot product.
         */
        float dot(const Vector3& v) const
        {
            return (x * v.x + y * v.y + z * v.z);
        }

        /**
         * Returns the dot product between the specified vectors.
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         *
         * @return The dot product between the vectors.
         */
        static float dot(const Vector3& v1, const Vector3& v2)
        {
            return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
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
            return (x * x + y * y + z * z);
        }

        /**
         * Negates this vector.
         */
        void negate()
        {
            x = -x;
            y = -y;
            z = -z;
        }

        /**
         * Normalizes this vector.
         *
         * This method normalizes this Vector3 so that it is of
         * unit length (in other words, the length of the vector
         * after calling this method will be 1.0f). If the vector
         * already has unit length or if the length of the vector
         * is zero, this method does nothing.
         *
         * @return This vector, after the normalization occurs.
         */
        Vector3& normalize();

        /**
         * Normalizes this vector and stores the result in dst.
         *
         * If the vector already has unit length or if the length
         * of the vector is zero, this method simply copies the
         * current vector into dst.
         *
         * @param dst The destination vector.
         */
        void normalize(Vector3& dst) const;

        /**
         * Scales all elements of this vector by the specified value.
         *
         * @param scalar The scalar value.
         */
        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        /**
         * Sets the elements of this vector to the specified values.
         *
         * @param newX The new x coordinate.
         * @param newY The new y coordinate.
         * @param newZ The new z coordinate.
         */
        void set(float newX, float newY, float newZ)
        {
            x = newX;
            y = newY;
            z = newZ;
        }

        /**
         * Sets the elements of this vector from the values in the specified array.
         *
         * @param array An array containing the elements of the vector in the order x, y, z.
         */
        void set(const float* array);

        /**
         * Sets the elements of this vector to those in the specified vector.
         *
         * @param v The vector to copy.
         */
        void set(const Vector3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        /**
         * Sets this vector to the directional vector between the specified points.
         */
        void set(const Vector3& p1, const Vector3& p2)
        {
            x = p2.x - p1.x;
            y = p2.y - p1.y;
            z = p2.z - p1.z;
        }

        /**
         * Subtracts this vector and the specified vector as (this - v)
         * and stores the result in this vector.
         *
         * @param v The vector to subtract.
         */
        void subtract(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        /**
         * Subtracts the specified vectors and stores the result in dst.
         * The resulting vector is computed as (v1 - v2).
         *
         * @param v1 The first vector.
         * @param v2 The second vector.
         * @param dst The destination vector.
         */
        static void subtract(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
            dst.z = v1.z - v2.z;
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
        void smooth(const Vector3& target, float elapsedTime, float responseTime);

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
        inline Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        /**
         * Adds the given vector to this vector.
         *
         * @param v The vector to add.
         * @return This vector, after the addition occurs.
         */
        inline Vector3& operator+=(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        /**
         * Calculates the difference of this vector with the given vector.
         *
         * Note: this does not modify this vector.
         *
         * @param v The vector to subtract.
         * @return The vector difference.
         */
        inline Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        /**
         * Subtracts the given vector from this vector.
         *
         * @param v The vector to subtract.
         * @return This vector, after the subtraction occurs.
         */
        inline Vector3& operator-=(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        /**
         * Calculates the negation of this vector.
         *
         * Note: this does not modify this vector.
         *
         * @return The negation of this vector.
         */
        inline Vector3 operator-() const
        {
            return Vector3(-x, -y, -z);
        }

        /**
         * Calculates the scalar product of this vector with the given value.
         *
         * Note: this does not modify this vector.
         *
         * @param scalar The value to scale by.
         * @return The scaled vector.
         */
        inline Vector3 operator*(float scalar) const
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        /**
         * Scales this vector by the given value.
         *
         * @param scalar The value to scale by.
         * @return This vector, after the scale occurs.
         */
        inline Vector3& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
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
        inline Vector3 operator/(float scalar) const
        {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        inline Vector3& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        /**
         * Determines if this vector is less than the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is less than the given vector, false otherwise.
         */
        inline bool operator<(const Vector3& v) const
        {
            if (x == v.x)
            {
                if (y == v.y)
                {
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
        inline bool operator==(const Vector3& v) const
        {
            return x==v.x && y==v.y && z==v.z;
        }

        /**
         * Determines if this vector is not equal to the given vector.
         *
         * @param v The vector to compare against.
         *
         * @return True if this vector is not equal to the given vector, false otherwise.
         */
        inline bool operator!=(const Vector3& v) const
        {
            return x!=v.x || y!=v.y || z!=v.z;
        }
    };

    /**
     * Calculates the scalar product of the given vector with the given value.
     *
     * @param scalar The value to scale by.
     * @param v The vector to scale.
     * @return The scaled vector.
     */
    inline Vector3 operator*(float scalar, const Vector3& v)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
}
