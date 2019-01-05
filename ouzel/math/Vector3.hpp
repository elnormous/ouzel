// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR3_HPP
#define OUZEL_MATH_VECTOR3_HPP

#include <cmath>
#include <cstddef>
#include "Vector2.hpp"

namespace ouzel
{
    template<class T> class Vector4;
    class Color;

    template<class T> class Vector3 final
    {
    public:
        T v[3]{0, 0, 0};

        Vector3()
        {
        }

        Vector3(T x, T y):
            v{x, y, 0}
        {
        }

        Vector3(T x, T y, T z):
            v{x, y, z}
        {
        }

        Vector3(const Vector3& copy):
            v{copy.v[0], copy.v[1], copy.v[2]}
        {
        }

        Vector3& operator=(const Vector3& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = vec.v[2];
            return *this;
        }

        explicit Vector3(const Vector2<T>& vec):
            v{vec.v[0], vec.v[1], 0.0F}
        {
        }

        Vector3& operator=(const Vector2<T>& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = 0.0F;
            return *this;
        }

        explicit Vector3(const Vector4<T>& vec);

        Vector3& operator=(const Vector4<T>& vec);

        explicit Vector3(Color color);
        Vector3& operator=(Color color);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }
        
        inline T& x() { return v[0]; }
        inline T x() const { return v[0]; }

        inline T& y() { return v[1]; }
        inline T y() const { return v[1]; }

        inline T& z() { return v[2]; }
        inline T z() const { return v[2]; }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0 && v[2] == 0;
        }

        float getAngle(const Vector3& axis) const
        {
            float dx = v[1] * axis.v[2] - v[2] * axis.v[1];
            float dy = v[2] * axis.v[0] - v[0] * axis.v[2];
            float dz = v[0] * axis.v[1] - v[1] * axis.v[0];

            return atan2f(sqrtf(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector3& min, const Vector3& max);

        Vector3 cross(const Vector3& vec) const
        {
            return Vector3((v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                           (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                           (v[0] * vec.v[1]) - (v[1] * vec.v[0]));
        }

        float distance(const Vector3& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            float dz = vec.v[2] - v[2];

            return sqrtf(dx * dx + dy * dy + dz * dz);
        }

        float distanceSquared(const Vector3& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            float dz = vec.v[2] - v[2];

            return dx * dx + dy * dy + dz * dz;
        }

        T dot(const Vector3& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2];
        }

        float length() const
        {
            return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

        float lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }

        void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

        void normalize();

        void scale(const Vector3& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
        }

        void smooth(const Vector3& target, T elapsedTime, T responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const;

        T getMax() const;

        inline const Vector3 operator+(const Vector2<T>& vec) const
        {
            return Vector3(v[0] + vec.v[0], v[1] + vec.v[1], v[2]);
        }

        inline const Vector3 operator+(const Vector3& vec) const
        {
            return Vector3(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2]);
        }

        inline Vector3& operator+=(const Vector2<T>& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            return *this;
        }

        inline Vector3& operator+=(const Vector3& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            return *this;
        }

        inline const Vector3 operator-(const Vector2<T>& vec) const
        {
            return Vector3(v[0] - vec.v[0], v[1] - vec.v[1], v[2]);
        }

        inline const Vector3 operator-(const Vector3& vec) const
        {
            return Vector3(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2]);
        }

        inline Vector3& operator-=(const Vector2<T>& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            return *this;
        }

        inline Vector3& operator-=(const Vector3& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
            return *this;
        }

        inline const Vector3 operator-() const
        {
            return Vector3(-v[0], -v[1], -v[2]);
        }

        inline const Vector3 operator*(T scalar) const
        {
            return Vector3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
        }

        inline Vector3& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            return *this;
        }

        inline const Vector3 operator/(T scalar) const
        {
            return Vector3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
        }

        inline Vector3& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            return *this;
        }

        inline bool operator<(const Vector3& vec) const
        {
            if (v[0] == vec.v[0])
            {
                if (v[1] == vec.v[1])
                    return v[2] < vec.v[2];

                return v[1] < vec.v[1];
            }
            return v[0] < vec.v[0];
        }

        inline bool operator==(const Vector3& vec) const
        {
            return v[0] == vec.v[0] && v[1] == vec.v[1] && v[2] == vec.v[2];
        }

        inline bool operator!=(const Vector3& vec) const
        {
            return v[0] != vec.v[0] || v[1] != vec.v[1] || v[2] != vec.v[2];
        }
    };

    template<class T>
    inline const Vector3<T> operator*(T scalar, const Vector3<T>& vec)
    {
        return Vector3<T>(vec.v[0] * scalar, vec.v[1] * scalar, vec.v[2] * scalar);
    }
}

#endif // OUZEL_MATH_VECTOR3_HPP
