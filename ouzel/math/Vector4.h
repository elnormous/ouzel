// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstddef>

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
        static const Vector4 NEGATIVE_UNIT_X;
        static const Vector4 NEGATIVE_UNIT_Y;
        static const Vector4 NEGATIVE_UNIT_Z;

#if OUZEL_SUPPORTS_SSE
        union
        {
            __m128 s;
            float v[4];
        };
#else
        float v[4];
#endif

        Vector4()
        {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            v[3] = 0.0f;
        }

        Vector4(float aX, float aY, float aZ, float aW)
        {
            v[0] = aX;
            v[1] = aY;
            v[2] = aZ;
            v[3] = aW;
        }

        Vector4(const Vector4& p1, const Vector4& p2);

        Vector4(const Vector4& copy)
        {
            v[0] = copy.v[0];
            v[1] = copy.v[1];
            v[2] = copy.v[2];
            v[3] = copy.v[3];
        }

        Vector4(const Vector2& v);
        Vector4& operator=(const Vector2& v);

        Vector4(const Vector3& v);
        Vector4& operator=(const Vector3& v);

        Vector4(const Color& color);
        Vector4& operator=(const Color& color);

        float& x() { return v[0]; }
        float& y() { return v[1]; }
        float& z() { return v[2]; }
        float& w() { return v[3]; }
        float x() const { return v[0]; }
        float y() const { return v[1]; }
        float z() const { return v[2]; }
        float w() const { return v[3]; }
        float& operator[](size_t index) { return v[index]; }
        float operator[](size_t index) const { return v[index]; }

        bool isZero() const
        {
            return v[0] == 0.0f && v[1] == 0.0f && v[2] == 0.0f && v[3] == 0.0f;
        }

        bool isOne() const
        {
            return v[0] == 1.0f && v[1] == 1.0f && v[2] == 1.0f && v[3] == 1.0f;
        }

        static float angle(const Vector4& v1, const Vector4& v2);

        void add(const Vector4& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            v[3] += vec.v[3];
        }

        static void add(const Vector4& v1, const Vector4& v2, Vector4& dst)
        {
            dst.v[0] = v1.v[0] + v2.v[0];
            dst.v[1] = v1.v[1] + v2.v[1];
            dst.v[2] = v1.v[2] + v2.v[2];
            dst.v[3] = v1.v[3] + v2.v[3];
        }

        void clamp(const Vector4& min, const Vector4& max);
        static void clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst);

        float distance(const Vector4& vec) const;

        float distanceSquared(const Vector4& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            float dz = vec.v[2] - v[2];
            float dw = vec.v[3] - v[3];

            return dx * dx + dy * dy + dz * dz + dw * dw;
        }

        float dot(const Vector4& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3] * vec.v[3];
        }

        static float dot(const Vector4& v1, const Vector4& v2)
        {
            return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1] + v1.v[2] * v2.v[2] + v1.v[3] * v2.v[3];
        }

        float length() const;

        float lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        }

        void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        Vector4& normalize();

        void scale(float scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;
        }

        void scale(const Vector4& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
            v[3] *= scale.v[3];
        }

        void set(float newX, float newY, float newZ, float newW)
        {
            v[0] = newX;
            v[1] = newY;
            v[2] = newZ;
            v[3] = newW;
        }

        void set(const Vector4& p1, const Vector4& p2)
        {
            v[0] = p2.v[0] - p1.v[0];
            v[1] = p2.v[1] - p1.v[1];
            v[2] = p2.v[2] - p1.v[2];
            v[3] = p2.v[3] - p1.v[3];
        }

        void subtract(const Vector4& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
            v[3] -= vec.v[3];
        }

        static void subtract(const Vector4& v1, const Vector4& v2, Vector4& dst)
        {
            dst.v[0] = v1.v[0] - v2.v[0];
            dst.v[1] = v1.v[1] - v2.v[1];
            dst.v[2] = v1.v[2] - v2.v[2];
            dst.v[3] = v1.v[3] - v2.v[3];
        }

        void smooth(const Vector4& target, float elapsedTime, float responseTime);

        float getMin() const;

        float getMax() const;

        inline Vector4 operator+(const Vector4& vec) const
        {
            Vector4 result(*this);
            result.add(vec);
            return result;
        }

        inline Vector4& operator+=(const Vector4& vec)
        {
            add(vec);
            return *this;
        }

        inline Vector4 operator-(const Vector4& vec) const
        {
            Vector4 result(*this);
            result.subtract(vec);
            return result;
        }

        inline Vector4& operator-=(const Vector4& vec)
        {
            subtract(vec);
            return *this;
        }

        inline Vector4 operator-() const
        {
            return Vector4(-v[0], -v[1], -v[2], -v[3]);
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
            return Vector4(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3] / scalar);
        }

        inline Vector4& operator/=(float scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            v[3] /= scalar;
            return *this;
        }

        inline bool operator<(const Vector4& vec) const
        {
            if (v[0] == vec.v[0])
            {
                if (v[1] == vec.v[1])
                {
                    if (v[2] == vec.v[2])
                    {
                        return v[3] < vec.v[3];
                    }
                    return v[2] < vec.v[2];
                }
                return v[1] < vec.v[1];
            }
            return v[0] < vec.v[0];
        }

        inline bool operator==(const Vector4& vec) const
        {
            return v[0] == vec.v[0] && v[1] == vec.v[1] && v[2] == vec.v[2] && v[3] == vec.v[3];
        }

        inline bool operator!=(const Vector4& vec) const
        {
            return v[0] != vec.v[0] || v[1] != vec.v[1] || v[2] != vec.v[2] || v[3] != vec.v[3];
        }
    };

    inline Vector4 operator*(float x, const Vector4& vec)
    {
        Vector4 result(vec);
        result.scale(x);
        return result;
    }
}
