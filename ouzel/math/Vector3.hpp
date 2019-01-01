// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR3_HPP
#define OUZEL_MATH_VECTOR3_HPP

#include <cstddef>

#include "Vector2.hpp"

namespace ouzel
{
    class Vector4;
    class Color;

    class Vector3 final
    {
    public:
        float v[3]{0.0F, 0.0F, 0.0F};

        Vector3()
        {
        }

        Vector3(float x, float y):
            v{x, y, 0.0F}
        {
        }

        Vector3(float x, float y, float z):
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

        explicit Vector3(const Vector2& vec):
            v{vec.v[0], vec.v[1], 0.0F}
        {
        }

        Vector3& operator=(const Vector2& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = 0.0F;
            return *this;
        }

        explicit Vector3(const Vector4& vec);

        Vector3& operator=(const Vector4& vec);

        explicit Vector3(Color color);
        Vector3& operator=(Color color);

        inline float& x() { return v[0]; }
        inline float& y() { return v[1]; }
        inline float& z() { return v[2]; }

        inline float x() const { return v[0]; }
        inline float y() const { return v[1]; }
        inline float z() const { return v[2]; }

        inline bool isZero() const
        {
            return v[0] == 0.0F && v[1] == 0.0F && v[2] == 0.0F;
        }

        static float angle(const Vector3& v1, const Vector3& v2);

        void add(const Vector3& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
        }

        static void add(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.v[0] = v1.v[0] + v2.v[0];
            dst.v[1] = v1.v[1] + v2.v[1];
            dst.v[2] = v1.v[2] + v2.v[2];
        }

        void clamp(const Vector3& min, const Vector3& max);
        static void clamp(const Vector3& vec, const Vector3& min, const Vector3& max, Vector3& dst);

        void cross(const Vector3& vec)
        {
            cross(*this, vec, *this);
        }

        static void cross(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.v[0] = (v1.v[1] * v2.v[2]) - (v1.v[2] * v2.v[1]);
            dst.v[1] = (v1.v[2] * v2.v[0]) - (v1.v[0] * v2.v[2]);
            dst.v[2] = (v1.v[0] * v2.v[1]) - (v1.v[1] * v2.v[0]);
        }

        static Vector3 cross(const Vector3& v1, const Vector3& v2)
        {
            Vector3 result;
            cross(v1, v2, result);
            return result;
        }

        float distance(const Vector3& vec) const;

        float distanceSquared(const Vector3& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            float dz = vec.v[2] - v[2];

            return dx * dx + dy * dy + dz * dz;
        }

        float dot(const Vector3& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2];
        }

        static float dot(const Vector3& v1, const Vector3& v2)
        {
            return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2];
        }

        float length() const;

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

        void scale(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
        }

        void scale(const Vector3& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
        }

        void set(float newX, float newY, float newZ)
        {
            v[0] = newX;
            v[1] = newY;
            v[2] = newZ;
        }

        void subtract(const Vector3& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
        }

        static void subtract(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.v[0] = v1.v[0] - v2.v[0];
            dst.v[1] = v1.v[1] - v2.v[1];
            dst.v[2] = v1.v[2] - v2.v[2];
        }

        void smooth(const Vector3& target, float elapsedTime, float responseTime);

        float getMin() const;

        float getMax() const;

        inline const Vector3 operator+(const Vector2& vec) const
        {
            return Vector3(v[0] + vec.v[0], v[1] + vec.v[1], v[2]);
        }

        inline const Vector3 operator+(const Vector3& vec) const
        {
            return Vector3(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2]);
        }

        inline Vector3& operator+=(const Vector2& vec)
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

        inline const Vector3 operator-(const Vector2& vec) const
        {
            return Vector3(v[0] - vec.v[0], v[1] - vec.v[1], v[2]);
        }

        inline const Vector3 operator-(const Vector3& vec) const
        {
            return Vector3(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2]);
        }

        inline Vector3& operator-=(const Vector2& vec)
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

        inline const Vector3 operator*(float scalar) const
        {
            return Vector3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
        }

        inline Vector3& operator*=(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            return *this;
        }

        inline const Vector3 operator/(float scalar) const
        {
            return Vector3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
        }

        inline Vector3& operator/=(float scalar)
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

    inline const Vector3 operator*(float scalar, const Vector3& vec)
    {
        return Vector3(vec.v[0] * scalar, vec.v[1] * scalar, vec.v[2] * scalar);
    }
}

#endif // OUZEL_MATH_VECTOR3_HPP
