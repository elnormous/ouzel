// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstddef>
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
        static const Vector2 NEGATIVE_UNIT_X;
        static const Vector2 NEGATIVE_UNIT_Y;

        float x = 0.0F;
        float y = 0.0F;

        Vector2()
        {
        }

        Vector2(float initX, float initY):
            x(initX), y(initY)
        {
        }

        Vector2(const Vector2& copy):
            x(copy.x), y(copy.y)
        {
        }

        Vector2(const Vector3& vec);

        Vector2& operator=(const Vector3& vec);

        Vector2(const Vector4& vec);

        Vector2& operator=(const Vector4& vec);

        inline bool isZero() const
        {
            return x == 0.0F && y == 0.0F;
        }

        inline bool isOne() const
        {
            return x == 1.0F && y == 1.0F;
        }

        static float angle(const Vector2& v1, const Vector2& v2);

        void add(const Vector2& vec)
        {
            x += vec.x;
            y += vec.y;
        }

        static void add(const Vector2& v1, const Vector2& v2, Vector2& dst);

        void clamp(const Vector2& min, const Vector2& max);

        static void clamp(const Vector2& vec, const Vector2& min, const Vector2& max, Vector2& dst);

        float distance(const Vector2& vec) const;

        float distanceSquared(const Vector2& vec) const
        {
            float dx = vec.x - x;
            float dy = vec.y - y;
            return dx * dx + dy * dy;
        }

        float dot(const Vector2& vec) const
        {
            return x * vec.x + y * vec.y;
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

        void normalize();

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

        void subtract(const Vector2& vec)
        {
            x -= vec.x;
            y -= vec.y;
        }

        static void subtract(const Vector2& v1, const Vector2& v2, Vector2& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
        }

        void smooth(const Vector2& target, float elapsedTime, float responseTime);

        float getMin() const;
        float getMax() const;

        inline Vector2 operator+(const Vector2& vec) const
        {
            return Vector2(x + vec.x, y + vec.y);
        }

        inline Vector2& operator+=(const Vector2& vec)
        {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        inline Vector2 operator-(const Vector2& vec) const
        {
            return Vector2(x - vec.x, y - vec.y);
        }

        inline Vector2& operator-=(const Vector2& vec)
        {
            x -= vec.x;
            y -= vec.y;
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

        inline bool operator<(const Vector2& vec) const
        {
            if (x == vec.x)
            {
                return y < vec.y;
            }
            return x < vec.x;
        }

        inline bool operator==(const Vector2& vec) const
        {
            return x == vec.x && y == vec.y;
        }

        inline bool operator!=(const Vector2& vec) const
        {
            return x != vec.x || y != vec.y;
        }

        inline float getAngle() const
        {
            return atan2f(y, x);
        };
    };

    inline Vector2 operator*(float scalar, const Vector2& vec)
    {
        return Vector2(vec.x * scalar, vec.y * scalar);
    }
}
