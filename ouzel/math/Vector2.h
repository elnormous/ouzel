// Copyright (C) 2017 Elviss Strazdins
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

        float v[2];

        Vector2():
            v{0.0f, 0.0f}
        {
        }

        Vector2(float aX, float aY):
            v{aX, aY}
        {
        }

        Vector2(const Vector2& p1, const Vector2& p2);

        Vector2(const Vector2& copy)
        {
            v[0] = copy.v[0];
            v[1] = copy.v[1];
        }

        Vector2(const Vector3& vec);

        Vector2& operator=(const Vector3& vec);

        Vector2(const Vector4& vec);

        Vector2& operator=(const Vector4& vec);

        float& x() { return v[0]; }
        float& y() { return v[1]; }
        float x() const { return v[0]; }
        float y() const { return v[1]; }
        float& operator[](size_t index) { return v[index]; }
        float operator[](size_t index) const { return v[index]; }

        inline bool isZero() const
        {
            return v[0] == 0.0f && v[1] == 0.0f;
        }

        inline bool isOne() const
        {
            return v[0] == 1.0f && v[1] == 1.0f;
        }

        static float angle(const Vector2& v1, const Vector2& v2);

        void add(const Vector2& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
        }

        static void add(const Vector2& v1, const Vector2& v2, Vector2& dst);

        void clamp(const Vector2& min, const Vector2& max);

        static void clamp(const Vector2& vec, const Vector2& min, const Vector2& max, Vector2& dst);

        float distance(const Vector2& vec) const;

        float distanceSquared(const Vector2& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            return dx * dx + dy * dy;
        }

        float dot(const Vector2& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1];
        }

        static float dot(const Vector2& v1, const Vector2& v2)
        {
            return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1];
        }

        float length() const;

        float lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1];
        }

        void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
        }

        Vector2& normalize();

        void scale(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
        }

        void scale(const Vector2& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
        }

        void rotate(float angle);
        void rotate(const Vector2& point, float angle);

        void set(float newX, float newY)
        {
            v[0] = newX;
            v[1] = newY;
        }

        void set(const Vector2& p1, const Vector2& p2)
        {
            v[0] = p2.v[0] - p1.v[0];
            v[1] = p2.v[1] - p1.v[1];
        }

        void subtract(const Vector2& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
        }

        static void subtract(const Vector2& v1, const Vector2& v2, Vector2& dst)
        {
            dst.v[0] = v1.v[0] - v2.v[0];
            dst.v[1] = v1.v[1] - v2.v[1];
        }

        void smooth(const Vector2& target, float elapsedTime, float responseTime);

        float getMin() const;
        float getMax() const;

        inline Vector2 operator+(const Vector2& vec) const
        {
            return Vector2(v[0] + vec.v[0], v[1] + vec.v[1]);
        }

        inline Vector2& operator+=(const Vector2& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            return *this;
        }

        inline Vector2 operator-(const Vector2& vec) const
        {
            return Vector2(v[0] - vec.v[0], v[1] - vec.v[1]);
        }

        inline Vector2& operator-=(const Vector2& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            return *this;
        }

        inline Vector2 operator-() const
        {
            return Vector2(-v[0], -v[1]);
        }

        inline Vector2 operator*(float scalar) const
        {
            return Vector2(v[0] * scalar, v[1] * scalar);
        }

        inline Vector2& operator*=(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            return *this;
        }

        inline Vector2 operator/(float scalar) const
        {
            return Vector2(v[0] / scalar, v[1] / scalar);
        }

        inline Vector2& operator/=(float scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            return *this;
        }

        inline bool operator<(const Vector2& vec) const
        {
            if (v[0] == vec.v[0])
            {
                return v[1] < vec.v[1];
            }
            return v[0] < vec.v[0];
        }

        inline bool operator==(const Vector2& vec) const
        {
            return v[0] == vec.v[0] && v[1] == vec.v[1];
        }

        inline bool operator!=(const Vector2& vec) const
        {
            return v[0] != vec.v[0] || v[1] != vec.v[1];
        }

        inline float getAngle() const
        {
            return atan2f(v[1], v[0]);
        };
    };

    inline Vector2 operator*(float scalar, const Vector2& vec)
    {
        return Vector2(vec.v[0] * scalar, vec.v[1] * scalar);
    }
}
