// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_QUATERNION_HPP
#define OUZEL_MATH_QUATERNION_HPP

#include <cmath>
#include "math/Vector3.hpp"

namespace ouzel
{
    template<class T> class Quaternion final
    {
    public:
        T v[4]{0, 0, 0, 0};

        Quaternion()
        {
        }

        Quaternion(T x, T y, T z, T w):
            v{x, y, z, w}
        {
        }

        Quaternion(const Quaternion& copy):
            v{copy.v[0], copy.v[1], copy.v[2], copy.v[3]}
        {
        }

        inline T& x() { return v[0]; }
        inline T& y() { return v[1]; }
        inline T& z() { return v[2]; }
        inline T& w() { return v[3]; }

        inline T x() const { return v[0]; }
        inline T y() const { return v[1]; }
        inline T z() const { return v[2]; }
        inline T w() const { return v[3]; }

        static inline Quaternion identity()
        {
            return Quaternion(0, 0, 0, 1);
        }

        inline const Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion(v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0],
                              -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1],
                              v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2],
                              -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3]);
        }

        inline Quaternion& operator*=(const Quaternion& q)
        {
            T tempX = v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0];
            T tempY = -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1];
            T tempZ = v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2];
            T tempW = -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3];

            v[0] = tempX;
            v[1] = tempY;
            v[2] = tempZ;
            v[3] = tempW;

            return *this;
        }

        inline const Quaternion operator*(T scalar) const
        {
            return Quaternion(v[0] * scalar,
                              v[1] * scalar,
                              v[2] * scalar,
                              v[3] * scalar);
        }

        inline Quaternion& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;

            return *this;
        }

        inline const Quaternion operator/(T scalar) const
        {
            return Quaternion(v[0] / scalar,
                              v[1] / scalar,
                              v[2] / scalar,
                              v[3] / scalar);
        }

        inline Quaternion& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            v[3] /= scalar;

            return *this;
        }

        inline const Quaternion operator-() const
        {
            return Quaternion(-v[0], -v[1], -v[2], -v[3]);
        }

        inline const Quaternion operator+(const Quaternion& q) const
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

        inline const Quaternion operator-(const Quaternion& q) const
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

        inline void invert()
        {
            T n2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; // norm squared

            if (n2 == 0.0F) return;

            // conjugate divided by norm squared
            v[0] = -v[0] / n2;
            v[1] = -v[1] / n2;
            v[2] = -v[2] / n2;
            v[3] = v[3] / n2;
        }

        float getNorm();
        void normalize();
        void rotate(float angle, Vector3<T> axis);
        void getRotation(float& angle, Vector3<T>& axis);

        Vector3<T> getEulerAngles() const
        {
            Vector3<T> result;
            result.v[0] = atan2f(2.0F * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
            result.v[1] = asinf(-2.0F * (v[0] * v[2] - v[3] * v[1]));
            result.v[2] = atan2f(2.0F * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
            return result;
        }

        float getEulerAngleX() const
        {
            return atan2f(2.0F * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
        }

        float getEulerAngleY() const
        {
            return asinf(-2.0F * (v[0] * v[2] - v[3] * v[1]));
        }

        float getEulerAngleZ() const
        {
            return atan2f(2.0F * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
        }
        
        void setEulerAngles(const Vector3<T>& angles);

        inline const Vector3<T> operator*(const Vector3<T>& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector3<T> rotateVector(const Vector3<T>& vector) const
        {
            Vector3<T> q(v[0], v[1], v[2]);
            Vector3<T> t = 2.0F * Vector3<T>::cross(q, vector);
            return vector + (v[3] * t) + Vector3<T>::cross(q, t);
        }

        inline Vector3<T> getRightVector() const
        {
            return rotateVector(Vector3<T>(1.0F, 0.0F, 0.0F));
        }

        inline Vector3<T> getUpVector() const
        {
            return rotateVector(Vector3<T>(0.0F, 1.0F, 0.0F));
        }

        inline Vector3<T> getForwardVector() const
        {
            return rotateVector(Vector3<T>(0.0F, 0.0F, 1.0F));
        }

        inline Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, float t)
        {
            const float scale = 1.0F - t;
            return (*this = (q1 * scale) + (q2 * t));
        }
    };
}

#endif // OUZEL_MATH_QUATERNION_HPP
