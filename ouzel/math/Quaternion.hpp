// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "math/Vector3.hpp"

namespace ouzel
{
    class Quaternion final
    {
    public:
        float x = 0.0F;
        float y = 0.0F;
        float z = 0.0F;
        float w = 0.0F;

        Quaternion()
        {
        }

        Quaternion(float initX, float initY, float initZ, float initW):
            x(initX), y(initY), z(initZ), w(initW)
        {
        }

        Quaternion(const Quaternion& copy):
            x(copy.x), y(copy.y), z(copy.z), w(copy.w)
        {
        }

        static inline Quaternion identity()
        {
            return Quaternion(0.0F, 0.0F, 0.0F, 1.0F);
        }

        inline const Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion( x * q.w + y * q.z - z * q.y + w * q.x,
                              -x * q.z + y * q.w + z * q.x + w * q.y,
                               x * q.y - y * q.x + z * q.w + w * q.z,
                              -x * q.x - y * q.y - z * q.z + w * q.w);
        }

        inline Quaternion& operator*=(const Quaternion& q)
        {
            float tempX =  x * q.w + y * q.z - z * q.y + w * q.x;
            float tempY = -x * q.z + y * q.w + z * q.x + w * q.y;
            float tempZ =  x * q.y - y * q.x + z * q.w + w * q.z;
            float tempW = -x * q.x - y * q.y - z * q.z + w * q.w;

            x = tempX;
            y = tempY;
            z = tempZ;
            w = tempW;

            return *this;
        }

        inline const Quaternion operator*(float scalar) const
        {
            return Quaternion(x * scalar,
                              y * scalar,
                              z * scalar,
                              w * scalar);
        }

        inline Quaternion& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;

            return *this;
        }

        inline const Quaternion operator/(float scalar) const
        {
            return Quaternion(x / scalar,
                              y / scalar,
                              z / scalar,
                              w / scalar);
        }

        inline Quaternion& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;

            return *this;
        }

        inline const Quaternion operator-() const
        {
            return Quaternion(-x, -y, -z, -w);
        }

        inline const Quaternion operator+(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.x += q.x;
            result.y += q.y;
            result.z += q.z;
            result.w += q.w;

            return result;
        }

        inline Quaternion& operator+=(const Quaternion& q)
        {
            x += q.x;
            y += q.y;
            z += q.z;
            w += q.w;

            return *this;
        }

        inline const Quaternion operator-(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.x -= q.x;
            result.y -= q.y;
            result.z -= q.z;
            result.w -= q.w;

            return result;
        }

        inline Quaternion& operator-=(const Quaternion& q)
        {
            x -= q.x;
            y -= q.y;
            z -= q.z;
            w -= q.w;

            return *this;
        }

        inline bool operator==(const Quaternion& q) const
        {
            return x == q.x && y == q.y && z == q.z && w == q.w;
        }

        inline bool operator!=(const Quaternion& q) const
        {
            return x != q.x || y != q.y || z != q.z || w != q.w;
        }

        inline void negate()
        {
            x = -x;
            y = -y;
            z = -z;
            w = -w;
        }

        inline void conjugate()
        {
            x = -x;
            y = -y;
            z = -z;
        }

        inline void invert()
        {
            float n2 = x * x + y * y + z * z + w * w; // norm squared

            if (n2 == 0.0F) return;

            // conjugate divided by norm squared
            x = -x / n2;
            y = -y / n2;
            z = -z / n2;
            w = w / n2;
        }

        float getNorm();
        void normalize();
        void rotate(float angle, Vector3 axis);
        void getRotation(float& angle, Vector3& axis);

        void setEulerAngles(const Vector3& angles);
        Vector3 getEulerAngles() const;
        float getEulerAngleX() const;
        float getEulerAngleY() const;
        float getEulerAngleZ() const;

        inline const Vector3 operator*(const Vector3& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector3 rotateVector(const Vector3& vector) const
        {
            Vector3 q(x, y, z);
            Vector3 t = 2.0F * Vector3::cross(q, vector);
            return vector + (w * t) + Vector3::cross(q, t);
        }

        inline Vector3 getRightVector() const
        {
            return rotateVector(Vector3(1.0F, 0.0F, 0.0F));
        }

        inline Vector3 getUpVector() const
        {
            return rotateVector(Vector3(0.0F, 1.0F, 0.0F));
        }

        inline Vector3 getForwardVector() const
        {
            return rotateVector(Vector3(0.0F, 0.0F, 1.0F));
        }

        inline Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, float t)
        {
            const float scale = 1.0F - t;
            return (*this = (q1 * scale) + (q2 * t));
        }
    };
}
