// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Matrix4.h"
#include "math/Vector3.h"

namespace ouzel
{
    class Quaternion
    {
    public:
        static const Quaternion IDENTITY;
        static const Quaternion ZERO;

        float v[4];

        Quaternion()
        {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            v[3] = 0.0f;
        }

        Quaternion(float aX, float aY, float aZ, float aW)
        {
            v[0] = aX;
            v[1] = aY;
            v[2] = aZ;
            v[3] = aW;
        }

        Quaternion(const Quaternion& copy)
        {
            v[0] = copy.v[0];
            v[1] = copy.v[1];
            v[2] = copy.v[2];
            v[3] = copy.v[3];
        }

        float& x() { return v[0]; }
        float& y() { return v[1]; }
        float& z() { return v[2]; }
        float& w() { return v[3]; }
        float x() const { return v[0]; }
        float y() const { return v[1]; }
        float z() const { return v[2]; }
        float w() const { return v[3]; }
        float& operator[](size_t index) { return v[index]; }
        float operator[](size_t index) const { return v[index]; }

        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion(v[3] * q.v[0] + q.v[3] * v[0] + v[1] * q.v[2] - v[2] * q.v[1],
                              v[3] * q.v[1] + q.v[3] * v[1] + v[2] * q.v[0] - v[0] * q.v[2],
                              v[3] * q.v[2] + q.v[3] * v[2] + v[0] * q.v[1] - v[1] * q.v[0],
                              v[3] * q.v[3] - (v[0] * q.v[0] + v[1] * q.v[1] + v[2] * q.v[2]));
        }

        const Quaternion& operator*=(const Quaternion& q)
        {
            float tempX = v[3] * q.v[0] + q.v[3] * v[0] + v[1] * q.v[2] - v[2] * q.v[1];
            float tempY = v[3] * q.v[1] + q.v[3] * v[1] + v[2] * q.v[0] - v[0] * q.v[2];
            float tempZ = v[3] * q.v[2] + q.v[3] * v[2] + v[0] * q.v[1] - v[1] * q.v[0];
            float tempW = v[3] * q.v[3] - (v[0] * q.v[0] + v[1] * q.v[1] + v[2] * q.v[2]);

            v[0] = tempX;
            v[1] = tempY;
            v[2] = tempZ;
            v[3] = tempW;

            return *this;
        }

        Quaternion operator*(float scalar) const
        {
            return Quaternion(v[0] * scalar,
                              v[1] * scalar,
                              v[2] * scalar,
                              v[3] * scalar);
        }

        const Quaternion& operator*=(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;

            return *this;
        }

        inline Quaternion operator-() const
        {
            return Quaternion(-v[0], -v[1], -v[2], -v[3]);
        }

        inline Quaternion operator+(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.v[0] += q.v[0];
            result.v[1] += q.v[1];
            result.v[2] += q.v[2];
            result.v[3] += q.v[3];

            return result;
        }

        inline Quaternion& operator+=(const Quaternion& q)
        {
            v[0] += q.v[0];
            v[1] += q.v[1];
            v[2] += q.v[2];
            v[3] += q.v[3];

            return *this;
        }

        inline Quaternion operator-(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.v[0] -= q.v[0];
            result.v[1] -= q.v[1];
            result.v[2] -= q.v[2];
            result.v[3] -= q.v[3];

            return result;
        }

        inline Quaternion& operator-=(const Quaternion& q)
        {
            v[0] -= q.v[0];
            v[1] -= q.v[1];
            v[2] -= q.v[2];
            v[3] -= q.v[3];

            return *this;
        }

        inline bool operator==(const Quaternion& q) const
        {
            return v[0] == q.v[0] && v[1] == q.v[1] && v[2] == q.v[2] && v[3] == q.v[3];
        }

        inline bool operator!=(const Quaternion& q) const
        {
            return v[0] != q.v[0] || v[1] != q.v[1] || v[2] != q.v[2] || v[3] != q.v[3];
        }

        inline void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        inline void conjugate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

        Quaternion& normalize();
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
            Vector3 q(v[0], v[1], v[2]);
            Vector3 t = 2.0f * Vector3::cross(q, vector);
            Vector3 result = vector + (v[3] * t) + Vector3::cross(q, t);
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

            float wx = v[3] * v[0];
            float wy = v[3] * v[1];
            float wz = v[3] * v[2];

            float xx = v[0] * v[0];
            float xy = v[0] * v[1];
            float xz = v[0] * v[2];

            float yy = v[1] * v[1];
            float yz = v[1] * v[2];

            float zz = v[2] * v[2];

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
