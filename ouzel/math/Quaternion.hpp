// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_QUATERNION_HPP
#define OUZEL_MATH_QUATERNION_HPP

#include <cmath>
#include <cstddef>
#include <limits>
#include "math/Vector.hpp"

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

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& x() { return v[0]; }
        inline T x() const { return v[0]; }

        inline T& y() { return v[1]; }
        inline T y() const { return v[1]; }

        inline T& z() { return v[2]; }
        inline T z() const { return v[2]; }

        inline T& w() { return v[3]; }
        inline T w() const { return v[3]; }

        static inline constexpr Quaternion identity()
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

        float getNorm() const
        {
            float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1.0F) // already normalized
                return 1.0F;

            return sqrtf(n);
        }

        void normalize()
        {
            float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return;

            n = 1.0F / n;
            v[0] *= n;
            v[1] *= n;
            v[2] *= n;
            v[3] *= n;
        }

        void rotate(float angle, Vector3<T> axis)
        {
            axis.normalize();

            float cosAngle = cosf(angle / 2.0F);
            float sinAngle = sinf(angle / 2.0F);

            v[0] = axis.v[0] * sinAngle;
            v[1] = axis.v[1] * sinAngle;
            v[2] = axis.v[2] * sinAngle;
            v[3] = cosAngle;
        }

        void getRotation(float& angle, Vector3<T>& axis) const
        {
            angle = 2.0F * acosf(v[3]);
            float s = sqrtf(1.0F - v[3] * v[3]);
            if (s < std::numeric_limits<float>::min()) // too close to zero
            {
                axis.v[0] = v[0];
                axis.v[1] = v[1];
                axis.v[2] = v[2];
            }
            else
            {
                axis.v[0] = v[0] / s;
                axis.v[1] = v[1] / s;
                axis.v[2] = v[2] / s;
            }
        }

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

        void setEulerAngles(const Vector3<T>& angles)
        {
            float angle;

            angle = angles.v[0] * 0.5F;
            const float sr = sinf(angle);
            const float cr = cosf(angle);

            angle = angles.v[1] * 0.5F;
            const float sp = sinf(angle);
            const float cp = cosf(angle);

            angle = angles.v[2] * 0.5F;
            const float sy = sinf(angle);
            const float cy = cosf(angle);

            const float cpcy = cp * cy;
            const float spcy = sp * cy;
            const float cpsy = cp * sy;
            const float spsy = sp * sy;

            v[0] = sr * cpcy - cr * spsy;
            v[1] = cr * spcy + sr * cpsy;
            v[2] = cr * cpsy - sr * spcy;
            v[3] = cr * cpcy + sr * spsy;
        }

        inline const Vector3<T> operator*(const Vector3<T>& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector3<T> rotateVector(const Vector3<T>& vector) const
        {
            Vector3<T> q(v[0], v[1], v[2]);
            Vector3<T> t = 2.0F * q.cross(vector);
            return vector + (v[3] * t) + q.cross(t);
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

    using QuaternionF = Quaternion<float>;
}

#endif // OUZEL_MATH_QUATERNION_HPP
