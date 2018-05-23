// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstddef>

namespace ouzel
{
    class Vector2;
    class Vector4;
    class Color;

    class Vector3
    {
    public:
        static const Vector3 ZERO;
        static const Vector3 ONE;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
        static const Vector3 NEGATIVE_UNIT_X;
        static const Vector3 NEGATIVE_UNIT_Y;
        static const Vector3 NEGATIVE_UNIT_Z;

        float x = 0.0F;
        float y = 0.0F;
        float z = 0.0F;

        Vector3()
        {
        }

        Vector3(float initX, float initY, float initZ):
            x(initX), y(initY), z(initZ)
        {
        }

        Vector3(const Vector3& copy):
            x(copy.x), y(copy.y), z(copy.z)
        {
        }

        Vector3(const Vector2& vec);
        Vector3(const Vector2& vec, float aZ);

        Vector3& operator=(const Vector2& vec);

        Vector3(const Vector4& vec);

        Vector3& operator=(const Vector4& vec);

        Vector3(const Color& color);
        Vector3& operator=(const Color& color);

        inline bool isZero() const
        {
            return x == 0.0F && y == 0.0F && z == 0.0F;
        }

        inline bool isOne() const
        {
            return x == 1.0F && y == 1.0F && z == 1.0F;
        }

        static float angle(const Vector3& v1, const Vector3& v2);

        void add(const Vector3& vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
        }

        static void add(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = v1.x + v2.x;
            dst.y = v1.y + v2.y;
            dst.z = v1.z + v2.z;
        }

        void clamp(const Vector3& min, const Vector3& max);
        static void clamp(const Vector3& vec, const Vector3& min, const Vector3& max, Vector3& dst);

        void cross(const Vector3& vec)
        {
            cross(*this, vec, *this);
        }

        static void cross(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = (v1.y * v2.z) - (v1.z * v2.y);
            dst.y = (v1.z * v2.x) - (v1.x * v2.z);
            dst.z = (v1.x * v2.y) - (v1.y * v2.x);
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
            float dx = vec.x - x;
            float dy = vec.y - y;
            float dz = vec.z - z;

            return dx * dx + dy * dy + dz * dz;
        }

        float dot(const Vector3& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        static float dot(const Vector3& v1, const Vector3& v2)
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }

        float length() const;

        float lengthSquared() const
        {
            return x * x + y * y + z * z;
        }

        void negate()
        {
            x = -x;
            y = -y;
            z = -z;
        }

        void normalize();

        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        void scale(const Vector3& scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
        }

        void set(float newX, float newY, float newZ)
        {
            x = newX;
            y = newY;
            z = newZ;
        }

        void subtract(const Vector3& vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
        }

        static void subtract(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
            dst.z = v1.z - v2.z;
        }

        void smooth(const Vector3& target, float elapsedTime, float responseTime);

        float getMin() const;

        float getMax() const;

        inline Vector3 operator+(const Vector3& vec) const
        {
            return Vector3(x + vec.x, y + vec.y, z + vec.z);
        }

        inline Vector3& operator+=(const Vector3& vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        inline Vector3 operator-(const Vector3& vec) const
        {
            return Vector3(x - vec.x, y - vec.y, z - vec.z);
        }

        inline Vector3& operator-=(const Vector3& vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        inline Vector3 operator-() const
        {
            return Vector3(-x, -y, -z);
        }

        inline Vector3 operator*(float scalar) const
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        inline Vector3& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

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

        inline bool operator<(const Vector3& vec) const
        {
            if (x == vec.x)
            {
                if (y == vec.y)
                {
                    return z < vec.z;
                }
                return y < vec.y;
            }
            return x < vec.x;
        }

        inline bool operator==(const Vector3& vec) const
        {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        inline bool operator!=(const Vector3& vec) const
        {
            return x != vec.x || y != vec.y || z != vec.z;
        }
    };

    inline Vector3 operator*(float scalar, const Vector3& vec)
    {
        return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
    }
}
