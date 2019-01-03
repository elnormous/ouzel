// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR4_HPP
#define OUZEL_MATH_VECTOR4_HPP

#include <cmath>
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace ouzel
{
    class Color;

    class Vector4 final
    {
    public:
#if defined(__SSE__)
        alignas(16)
#endif
        float v[4]{0.0F, 0.0F, 0.0F, 0.0F};

        Vector4()
        {
        }

        Vector4(float x, float y):
            v{x, y, 0.0F, 0.0F}
        {
        }

        Vector4(float x, float y, float z):
            v{x, y, z, 0.0F}
        {
        }

        Vector4(float x, float y, float z, float w):
            v{x, y, z, w}
        {
        }

        Vector4(const Vector4& copy):
            v{copy.v[0], copy.v[1], copy.v[2], copy.v[3]}
        {
        }

        Vector4& operator=(const Vector4& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = vec.v[2];
            v[3] = vec.v[3];
            return *this;
        }

        explicit Vector4(const Vector2& vec):
            v{vec.v[0], vec.v[1], 0.0F, 0.0F}
        {
        }

        Vector4& operator=(const Vector2& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = 0.0F;
            v[3] = 0.0F;
            return *this;
        }

        explicit Vector4(const Vector3& vec):
            v{vec.v[0], vec.v[1], vec.v[2], 0.0F}
        {
        }

        Vector4& operator=(const Vector3& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = vec.v[2];
            v[3] = 0.0F;
            return *this;
        }

        explicit Vector4(Color color);
        Vector4& operator=(Color color);

        inline float& x() { return v[0]; }
        inline float& y() { return v[1]; }
        inline float& z() { return v[2]; }
        inline float& w() { return v[3]; }

        inline float x() const { return v[0]; }
        inline float y() const { return v[1]; }
        inline float z() const { return v[2]; }
        inline float w() const { return v[3]; }

        bool isZero() const
        {
            return v[0] == 0.0F && v[1] == 0.0F && v[2] == 0.0F && v[3] == 0.0F;
        }

        static float angle(const Vector4& v1, const Vector4& v2)
        {
            float dx = v1.v[3] * v2.v[0] - v1.v[0] * v2.v[3] - v1.v[1] * v2.v[2] + v1.v[2] * v2.v[1];
            float dy = v1.v[3] * v2.v[1] - v1.v[1] * v2.v[3] - v1.v[2] * v2.v[0] + v1.v[0] * v2.v[2];
            float dz = v1.v[3] * v2.v[2] - v1.v[2] * v2.v[3] - v1.v[0] * v2.v[1] + v1.v[1] * v2.v[0];

            return atan2f(sqrtf(dx * dx + dy * dy + dz * dz), dot(v1, v2));
        }

        void clamp(const Vector4& min, const Vector4& max);
        static void clamp(const Vector4& vec, const Vector4& min, const Vector4& max, Vector4& dst);

        float distance(const Vector4& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            float dz = vec.v[2] - v[2];
            float dw = vec.v[3] - v[3];

            return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw);
        }

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

        inline float length() const
        {
            return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
        }

        inline float lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
        }

        inline void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        void normalize();

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

        void smooth(const Vector4& target, float elapsedTime, float responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        float getMin() const;

        float getMax() const;

        inline const Vector4 operator+(const Vector2& vec) const
        {
            return Vector4(v[0] + vec.v[0], v[1] + vec.v[1], v[2], v[3]);
        }

        inline const Vector4 operator+(const Vector3& vec) const
        {
            return Vector4(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2], v[3]);
        }

        inline const Vector4 operator+(const Vector4& vec) const
        {
            return Vector4(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2], v[3] + vec.v[3]);
        }

        inline Vector4& operator+=(const Vector2& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            return *this;
        }

        inline Vector4& operator+=(const Vector3& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            return *this;
        }

        inline Vector4& operator+=(const Vector4& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            v[3] += vec.v[3];
            return *this;
        }

        inline const Vector4 operator-(const Vector2& vec) const
        {
            return Vector4(v[0] - vec.v[0], v[1] - vec.v[1], v[2], v[3]);
        }

        inline const Vector4 operator-(const Vector3& vec) const
        {
            return Vector4(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2], v[3]);
        }

        inline const Vector4 operator-(const Vector4& vec) const
        {
            return Vector4(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2], v[3] - vec.v[3]);
        }

        inline Vector4& operator-=(const Vector2& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            return *this;
        }

        inline Vector4& operator-=(const Vector3& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
            return *this;
        }

        inline Vector4& operator-=(const Vector4& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
            v[3] -= vec.v[3];
            return *this;
        }

        inline const Vector4 operator-() const
        {
            return Vector4(-v[0], -v[1], -v[2], -v[3]);
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
                        return v[3] < vec.v[3];

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

    inline const Vector4 operator*(float scalar, const Vector4& vec)
    {
        Vector4 result(vec);
        result.scale(scalar);
        return result;
    }
}

#endif // OUZEL_MATH_VECTOR4_HPP
