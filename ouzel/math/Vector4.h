// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"
#if OUZEL_SUPPORTS_SSE
#include <xmmintrin.h>
#endif

namespace ouzel
{
    class Vector2;
    class Vector3;
    class Color;

    class Vector4
    {
    public:
        static const Vector4 ZERO;
        static const Vector4 ONE;
        static const Vector4 UNIT_X;
        static const Vector4 UNIT_Y;
        static const Vector4 UNIT_Z;
        static const Vector4 UNIT_W;

        float x;
        float y;
        float z;
        float w;

        Vector4():
            x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
        }

        Vector4(float aX, float aY, float aZ, float aW):
            x(aX), y(aY), z(aZ), w(aW)
        {
        }

        Vector4(const Vector4& p1, const Vector4& p2);

        Vector4(const Vector4& copy):
            x(copy.x),
            y(copy.y),
            z(copy.z),
            w(copy.w)
        {
        }

        Vector4(const Vector2& v);

        Vector4& operator=(const Vector2& v);

        Vector4(const Vector3& v);

        Vector4& operator=(const Vector3& v);

        static Vector4 fromColor(const Color& color);

        bool isZero() const
        {
            return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
        }

        bool isOne() const
        {
            return x == 1.0f && y == 1.0f && z == 1.0f && w == 1.0f;
        }

        static float angle(const Vector4& v1, const Vector4& v2);

        void add(const Vector4& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
        }

        static void add(const Vector4& v1, const Vector4& v2, Vector4& dst)
        {
            dst.x = v1.x + v2.x;
            dst.y = v1.y + v2.y;
            dst.z = v1.z + v2.z;
            dst.w = v1.w + v2.w;
        }

        void clamp(const Vector4& min, const Vector4& max);
        static void clamp(const Vector4& v, const Vector4& min, const Vector4& max, Vector4& dst);

        float distance(const Vector4& v) const;

        float distanceSquared(const Vector4& v) const
        {
            float dx = v.x - x;
            float dy = v.y - y;
            float dz = v.z - z;
            float dw = v.w - w;

            return dx * dx + dy * dy + dz * dz + dw * dw;
        }

        float dot(const Vector4& v) const
        {
            return x * v.x + y * v.y + z * v.z + w * v.w;
        }

        static float dot(const Vector4& v1, const Vector4& v2)
        {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }

        float length() const;

        float lengthSquared() const
        {
            return x * x + y * y + z * z + w * w;
        }

        void negate()
        {
            x = -x;
            y = -y;
            z = -z;
            w = -w;
        }

        Vector4& normalize();
        void normalize(Vector4& dst) const;

        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
        }

        void set(float newX, float newY, float newZ, float newW)
        {
            x = newX;
            y = newY;
            z = newZ;
            w = newW;
        }

        void set(const Vector4& p1, const Vector4& p2)
        {
            x = p2.x - p1.x;
            y = p2.y - p1.y;
            z = p2.z - p1.z;
            w = p2.w - p1.w;
        }

        void subtract(const Vector4& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
        }

        static void subtract(const Vector4& v1, const Vector4& v2, Vector4& dst)
        {
            dst.x = v1.x - v2.x;
            dst.y = v1.y - v2.y;
            dst.z = v1.z - v2.z;
            dst.w = v1.w - v2.w;
        }

        void smooth(const Vector4& target, float elapsedTime, float responseTime);

        float getMin() const;

        float getMax() const;

        inline Vector4 operator+(const Vector4& v) const
        {
            Vector4 result(*this);
            result.add(v);
            return result;
        }

        inline Vector4& operator+=(const Vector4& v)
        {
            add(v);
            return *this;
        }

        inline Vector4 operator-(const Vector4& v) const
        {
            Vector4 result(*this);
            result.subtract(v);
            return result;
        }

        inline Vector4& operator-=(const Vector4& v)
        {
            subtract(v);
            return *this;
        }

        inline Vector4 operator-() const
        {
            return Vector4(-x, -y, -z, -w);
        }

        inline Vector4 operator*(float scalar) const
        {
            Vector4 result(*this);
            result.scale(scalar);
            return result;
        }

        inline Vector4& operator*=(float scalar)
        {
            scale(scalar);
            return *this;
        }

        inline Vector4 operator/(float scalar) const
        {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        inline Vector4& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        inline bool operator<(const Vector4& v) const
        {
            if (x == v.x)
            {
                if (y == v.y)
                {
                    if (z == v.z)
                    {
                        return w < v.w;
                    }
                    return z < v.z;
                }
                return y < v.y;
            }
            return x < v.x;
        }

        inline bool operator==(const Vector4& v) const
        {
            return x == v.x && y == v.y && z == v.z && w == v.w;
        }

        inline bool operator!=(const Vector4& v) const
        {
            return x != v.x || y != v.y || z != v.z || w != v.w;
        }
    };

    inline Vector4 operator*(float x, const Vector4& v)
    {
        Vector4 result(v);
        result.scale(x);
        return result;
    }
}
