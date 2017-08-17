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

        union
        {
            struct
            {
                float x = 0.0f;
                float y = 0.0f;
                float z = 0.0f;
                float w = 0.0f;
            };
            float v[4];
        };

        Quaternion()
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
