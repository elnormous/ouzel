// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Matrix4.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Quaternion
    {
    public:
        static const Quaternion IDENTITY;
        static const Quaternion ZERO;

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        Quaternion():
            x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
        }

        Quaternion(float aX, float aY, float aZ, float aW):
            x(aX), y(aY), z(aZ), w(aW)
        {
        }

        Quaternion(const Quaternion& copy):
            x(copy.x), y(copy.y), z(copy.z), w(copy.w)
        {
        }

        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion(w * q.x + q.w * x + y * q.z - z * q.y,
                              w * q.y + q.w * y + z * q.x - x * q.z,
                              w * q.z + q.w * z + x * q.y - y * q.x,
                              w * q.w - (x * q.x + y * q.y + z * q.z));
        }

        const Quaternion& operator*=(const Quaternion& q)
        {
            float tempX = w * q.x + q.w * x + y * q.z - z * q.y;
            float tempY = w * q.y + q.w * y + z * q.x - x * q.z;
            float tempZ = w * q.z + q.w * z + x * q.y - y * q.x;
            float tempW = w * q.w - (x * q.x + y * q.y + z * q.z);

            x = tempX;
            y = tempY;
            z = tempZ;
            w = tempW;

            return *this;
        }

        Quaternion operator*(float scalar) const
        {
            return Quaternion(x * scalar,
                              y * scalar,
                              z * scalar,
                              w * scalar);
        }

        const Quaternion& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;

            return *this;
        }

        inline Quaternion operator-() const
        {
            return Quaternion(-x, -y, -z, -w);
        }

        inline Quaternion operator+(const Quaternion& q) const
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

        inline Quaternion operator-(const Quaternion& q) const
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

        void normalize();
        void rotate(float angle, Vector3 axis);

        void setEulerAngles(const Vector3& angles);
        Vector3 getEulerAngles() const;
        float getEulerAngleX() const;
        float getEulerAngleY() const;
        float getEulerAngleZ() const;

        inline Vector3 operator*(const Vector3& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector3 rotateVector(const Vector3& vector) const
        {
            Vector3 q(x, y, z);
            Vector3 t = 2.0f * Vector3::cross(q, vector);
            Vector3 result = vector + (w * t) + Vector3::cross(q, t);
            return result;
        }

        inline Vector3 getRightVector() const
        {
            return rotateVector(Vector3::UNIT_X);
        }

        inline Vector3 getUpVector() const
        {
            return rotateVector(Vector3::UNIT_Y);
        }

        inline Vector3 getForwardVector() const
        {
            return rotateVector(Vector3::UNIT_Z);
        }

        Matrix4 getMatrix() const
        {
            Matrix4 result;

            float wx = w * x;
            float wy = w * y;
            float wz = w * z;

            float xx = x * x;
            float xy = x * y;
            float xz = x * z;

            float yy = y * y;
            float yz = y * z;

            float zz = z * z;

            result.m[0] = 1.0f - 2.0f * (yy + zz);
            result.m[1] = 2.0f * (xy + wz);
            result.m[2] = 2.0f * (xz - wy);
            result.m[3] = 0.0f;

            result.m[4] = 2.0f * (xy - wz);
            result.m[5] = 1.0f - 2.0f * (xx + zz);
            result.m[6] = 2.0f * (yz + wx);
            result.m[7] = 0.0f;

            result.m[8] = 2.0f * (xz +  wy);
            result.m[9] = 2.0f * (yz - wx);
            result.m[10] = 1.0f - 2.0f * (xx + yy);
            result.m[11] = 0.0f;

            result.m[12] = 0.0f;
            result.m[13] = 0.0f;
            result.m[14] = 0.0f;
            result.m[15] = 1.0f;

            return result;
        }

        Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, float t)
        {
            const float scale = 1.0f - t;
            return (*this = (q1 * scale) + (q2 * t));
        }
    };
}
