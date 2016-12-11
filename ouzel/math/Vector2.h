// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cmath>

namespace ouzel
{
    class Vector3;
    class Vector4;

    class Vector2
    {
    public:
        static const Vector2 ZERO;
        static const Vector2 ONE;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;

        float x;
        float y;

        Vector2():
            x(0.0f), y(0.0f)
        {
        }

        Vector2(float aX, float aY):
            x(aX), y(aY)
        {
        }

        Vector2(const Vector2& p1, const Vector2& p2);

        Vector2(const Vector2& copy):
            x(copy.x),
            y(copy.y)
        {
        }

        Vector2(const Vector3& v);

        Vector2& operator=(const Vector3& v);

        Vector2(const Vector4& v);

        Vector2& operator=(const Vector4& v);

        inline bool isZero() const
        {
            return x == 0.0f && y == 0.0f;
        }

        inline bool isOne() const
        {
            return x == 1.0f && y == 1.0f;
        }

        static float angle(const Vector2& v1, const Vector2& v2);

        void add(const Vector2& v)
        {
            x += v.x;
            y += v.y;
        }

        static void add(const Vector2& v1, const Vector2& v2, Vector2& dst);

        void clamp(const Vector2& min, const Vector2& max);

        static void clamp(const Vector2& v, const Vector2& min, const Vector2& max, Vector2& dst);

        float distance(const Vector2& v) const;

        float distanceSquared(const Vector2& v) const
        {
            float dx = v.x - x;
            float dy = v.y - y;
            return dx * dx + dy * dy;
        }

        float dot(const Vector2& v) const
        {
            return x * v.x + y * v.y;
        }

        static float dot(const Vector2& v1, const Vector2& v2)
        {
            return v1.x * v2.x + v1.y * v2.y;
        }

        float length() const;

        float lengthSquared() const
        {
            return x * x + y * y;
        }

        void negate()
        {
            x = -x;
            y = -y;
        }

        Vector2& normalize();
        void normalize(Vector2& dst) const;

        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
        }

        void scale(const Vector2& scale)
        {
            x *= scale.x;
            y *= scale.y;
        }

        void rotate(float angle);
        void rotate(const Vector2& point, float angle);

        void set(float newX, float newY)
        {
            x = newX;
            y = newY;
        }

        void set(const Vector2& p1, const Vector2& p2)
        {
            x = p2.x - p1.x;
            y = p2.y - p1.y;
        }

        void subtract(const Vector2& v)
        {
            x -= v.x;
            y -= v.y;
        }

        static void subtract(const Vector2& v1, const Vector2& v2, Vector2& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
        }

        void smooth(const Vector2& target, float elapsedTime, float responseTime);

        float getMin() const;
        float getMax() const;

        inline Vector2 operator+(const Vector2& v) const
        {
            return Vector2(x + v.x, y + v.y);
        }

        inline Vector2& operator+=(const Vector2& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        inline Vector2 operator-(const Vector2& v) const
        {
            return Vector2(x - v.x, y - v.y);
        }

        inline Vector2& operator-=(const Vector2& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        inline Vector2 operator-() const
        {
            return Vector2(-x, -y);
        }

        inline Vector2 operator*(float scalar) const
        {
            return Vector2(x * scalar, y * scalar);
        }

        inline Vector2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        inline Vector2 operator/(float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
        }

        inline Vector2& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        inline bool operator<(const Vector2& v) const
        {
            if (x == v.x)
            {
                return y < v.y;
            }
            return x < v.x;
        }

        inline bool operator==(const Vector2& v) const
        {
            return x == v.x && y == v.y;
        }

        inline bool operator!=(const Vector2& v) const
        {
            return x != v.x || y != v.y;
        }

        inline float getAngle() const
        {
            return atan2f(y, x);
        };
    };

    inline Vector2 operator*(float scalar, const Vector2& v)
    {
        return Vector2(v.x * scalar, v.y * scalar);
    }
}
