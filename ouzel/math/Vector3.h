// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    class Matrix;
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

        float x;
        float y;
        float z;

        Vector3():
            x(0.0f), y(0.0f), z(0.0f)
        {
        }

        Vector3(float aX, float aY, float aZ):
            x(aX), y(aY), z(aZ)
        {
        }

        Vector3(const Vector3& p1, const Vector3& p2);

        Vector3(const Vector3& copy):
            x(copy.x),
            y(copy.y),
            z(copy.z)
        {
        }

        Vector3(const Vector2& v);
        Vector3(const Vector2& v, float aZ);

        Vector3& operator=(const Vector2& v);

        Vector3(const Vector4& v);

        Vector3& operator=(const Vector4& v);

        static Vector3 fromColor(const Color& color);

        inline bool isZero() const
        {
            return x == 0.0f && y == 0.0f && z == 0.0f;
        }

        inline bool isOne() const
        {
            return x == 1.0f && y == 1.0f && z == 1.0f;
        }

        static float angle(const Vector3& v1, const Vector3& v2);

        void add(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        static void add(const Vector3& v1, const Vector3& v2, Vector3& dst)
        {
            dst.x = v1.x + v2.x;
            dst.y = v1.y + v2.y;
            dst.z = v1.z + v2.z;
        }

        void clamp(const Vector3& min, const Vector3& max);
        static void clamp(const Vector3& v, const Vector3& min, const Vector3& max, Vector3& dst);

        void cross(const Vector3& v);
        static void cross(const Vector3& v1, const Vector3& v2, Vector3& dst);

        float distance(const Vector3& v) const;

        float distanceSquared(const Vector3& v) const
        {
            float dx = v.x - x;
            float dy = v.y - y;
            float dz = v.z - z;

            return dx * dx + dy * dy + dz * dz;
        }

        float dot(const Vector3& v) const
        {
            return x * v.x + y * v.y + z * v.z;
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

        Vector3& normalize();

        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
        }

        void set(float newX, float newY, float newZ)
        {
            x = newX;
            y = newY;
            z = newZ;
        }

        void set(const Vector3& p1, const Vector3& p2)
        {
            x = p2.x - p1.x;
            y = p2.y - p1.y;
            z = p2.z - p1.z;
        }

        void subtract(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
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

        inline Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x + v.x, y + v.y, z + v.z);
        }

        inline Vector3& operator+=(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        inline Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x - v.x, y - v.y, z - v.z);
        }

        inline Vector3& operator-=(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
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

        inline bool operator<(const Vector3& v) const
        {
            if (x == v.x)
            {
                if (y == v.y)
                {
                    return z < v.z;
                }
                return y < v.y;
            }
            return x < v.x;
        }

        inline bool operator==(const Vector3& v) const
        {
            return x == v.x && y == v.y && z == v.z;
        }

        inline bool operator!=(const Vector3& v) const
        {
            return x != v.x || y != v.y || z != v.z;
        }
    };

    inline Vector3 operator*(float scalar, const Vector3& v)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
}
