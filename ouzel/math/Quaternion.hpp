// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_QUATERNION_HPP
#define OUZEL_MATH_QUATERNION_HPP

#include <cmath>
#include <cstddef>
#include <limits>
#include "math/Vector.hpp"

namespace ouzel
{
    template <class T> class Quaternion final
    {
    public:
        T v[4]{0};

        Quaternion() = default;

        Quaternion(T x, T y, T z, T w):
            v{x, y, z, w}
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

            if (n2 <= std::numeric_limits<T>::min())
                return;

            // conjugate divided by norm squared
            v[0] = -v[0] / n2;
            v[1] = -v[1] / n2;
            v[2] = -v[2] / n2;
            v[3] = v[3] / n2;
        }

        inline T getNorm() const
        {
            T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1) // already normalized
                return 1;

            return sqrt(n);
        }

        void normalize()
        {
            T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1) // already normalized
                return;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return;

            n = 1 / n;
            v[0] *= n;
            v[1] *= n;
            v[2] *= n;
            v[3] *= n;
        }

        void rotate(T angle, Vector<3, T> axis)
        {
            axis.normalize();

            T cosAngle = cos(angle / 2);
            T sinAngle = sin(angle / 2);

            v[0] = axis.v[0] * sinAngle;
            v[1] = axis.v[1] * sinAngle;
            v[2] = axis.v[2] * sinAngle;
            v[3] = cosAngle;
        }

        void getRotation(T& angle, Vector<3, T>& axis) const
        {
            angle = 2 * acos(v[3]);
            T s = sqrt(1 - v[3] * v[3]);
            if (s <= std::numeric_limits<T>::min()) // too close to zero
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

        Vector<3, T> getEulerAngles() const
        {
            Vector<3, T> result;
            result.v[0] = atan2(2 * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
            result.v[1] = asin(-2 * (v[0] * v[2] - v[3] * v[1]));
            result.v[2] = atan2(2 * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
            return result;
        }

        inline T getEulerAngleX() const
        {
            return atan2(2 * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
        }

        inline T getEulerAngleY() const
        {
            return asin(-2 * (v[0] * v[2] - v[3] * v[1]));
        }

        inline T getEulerAngleZ() const
        {
            return atan2(2 * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
        }

        void setEulerAngles(const Vector<3, T>& angles)
        {
            T angle;

            angle = angles.v[0] / 2;
            const T sr = sin(angle);
            const T cr = cos(angle);

            angle = angles.v[1] / 2;
            const T sp = sin(angle);
            const T cp = cos(angle);

            angle = angles.v[2] / 2;
            const T sy = sin(angle);
            const T cy = cos(angle);

            const T cpcy = cp * cy;
            const T spcy = sp * cy;
            const T cpsy = cp * sy;
            const T spsy = sp * sy;

            v[0] = sr * cpcy - cr * spsy;
            v[1] = cr * spcy + sr * cpsy;
            v[2] = cr * cpsy - sr * spcy;
            v[3] = cr * cpcy + sr * spsy;
        }

        inline const Vector<3, T> operator*(const Vector<3, T>& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector<3, T> rotateVector(const Vector<3, T>& vector) const
        {
            Vector<3, T> q(v[0], v[1], v[2]);
            Vector<3, T> t = 2 * q.cross(vector);
            return vector + (v[3] * t) + q.cross(t);
        }

        inline Vector<3, T> getRightVector() const
        {
            return rotateVector(Vector<3, T>(1, 0, 0));
        }

        inline Vector<3, T> getUpVector() const
        {
            return rotateVector(Vector<3, T>(0, 1, 0));
        }

        inline Vector<3, T> getForwardVector() const
        {
            return rotateVector(Vector<3, T>(0, 0, 1));
        }

        inline Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, T t)
        {
            const T scale = 1 - t;
            *this = (q1 * scale) + (q2 * t);
            return *this;
        }
    };

    using QuaternionF = Quaternion<float>;
}

#endif // OUZEL_MATH_QUATERNION_HPP
