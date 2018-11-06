// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include <cstddef>

#include "core/Setup.h"
#if OUZEL_SUPPORTS_SSE
#include <xmmintrin.h>
#endif

namespace ouzel
{
    class Vector2;
    class Vector3;
    class Color;

    class Vector4 final
    {
    public:
#if OUZEL_SUPPORTS_SSE
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4201)
#endif
        union
        {
            __m128 s;

            struct
            {
                float x;
                float y;
                float z;
                float w;
            };
        };
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else
        float x;
        float y;
        float z;
        float w;
#endif

        Vector4():
            x(0.0F), y(0.0F), z(0.0F), w(0.0F)
        {
        }

        Vector4(float initX, float initY, float initZ, float initW):
            x(initX), y(initY), z(initZ), w(initW)
        {
        }

        Vector4(const Vector4& copy):
            x(copy.x), y(copy.y), z(copy.z), w(copy.w)
        {
        }

        explicit Vector4(const Vector2& v);
        Vector4& operator=(const Vector2& v);

        explicit Vector4(const Vector3& v);
        Vector4& operator=(const Vector3& v);

        explicit Vector4(Color color);
        Vector4& operator=(Color color);

        bool isZero() const
        {
            return x == 0.0F && y == 0.0F && z == 0.0F && w == 0.0F;
        }

        bool isOne() const
        {
            return x == 1.0F && y == 1.0F && z == 1.0F && w == 1.0F;
        }

        static float angle(const Vector4& v1, const Vector4& v2);

        void add(const Vector4& vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
        }

        static void add(const Vector4& v1, const Vector4& v2, Vector4& dst)
        {
            dst.x = v1.x + v2.x;
            dst.y = v1.y + v2.y;
            dst.z = v1.z + v2.z;
            dst.w = v1.w + v2.w;
        }

        void clamp(const Vector4& min, const Vector4& max);
        static void clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst);

        float distance(const Vector4& vec) const;

        float distanceSquared(const Vector4& vec) const
        {
            float dx = vec.x - x;
            float dy = vec.y - y;
            float dz = vec.z - z;
            float dw = vec.w - w;

            return dx * dx + dy * dy + dz * dz + dw * dw;
        }

        float dot(const Vector4& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
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

        void normalize();

        void scale(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
        }

        void scale(const Vector4& scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
            w *= scale.w;
        }

        void set(float newX, float newY, float newZ, float newW)
        {
            x = newX;
            y = newY;
            z = newZ;
            w = newW;
        }

        void subtract(const Vector4& vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
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

        inline const Vector4 operator+(const Vector4& vec) const
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

        inline const Vector4 operator-(const Vector4& vec) const
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

        inline const Vector4 operator-() const
        {
            return Vector4(-x, -y, -z, -w);
        }

        inline const Vector4 operator*(float scalar) const
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

        inline const Vector4 operator/(float scalar) const
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

        inline bool operator<(const Vector4& vec) const
        {
            if (x == vec.x)
            {
                if (y == vec.y)
                {
                    if (z == vec.z)
                        return w < vec.w;

                    return z < vec.z;
                }
                return y < vec.y;
            }
            return x < vec.x;
        }

        inline bool operator==(const Vector4& vec) const
        {
            return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
        }

        inline bool operator!=(const Vector4& vec) const
        {
            return x != vec.x || y != vec.y || z != vec.z || w != vec.w;
        }
    };

    inline const Vector4 operator*(float x, const Vector4& vec)
    {
        Vector4 result(vec);
        result.scale(x);
        return result;
    }
}

#endif // VECTOR4_HPP
