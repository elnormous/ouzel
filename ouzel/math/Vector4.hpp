// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR4_HPP
#define OUZEL_MATH_VECTOR4_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    class Color;

    template<class T> class Vector4 final
    {
    public:
#if defined(__SSE__)
        alignas(16)
#endif
        T v[4]{0, 0, 0, 0};

        Vector4()
        {
        }

        Vector4(T x, T y):
            v{x, y, 0, 0}
        {
        }

        Vector4(T x, T y, T z):
            v{x, y, z, 0}
        {
        }

        Vector4(T x, T y, T z, T w):
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

        explicit Vector4(const Vector2<T>& vec):
            v{vec.v[0], vec.v[1], 0.0F, 0.0F}
        {
        }

        explicit Vector4(const Vector3<T>& vec):
            v{vec.v[0], vec.v[1], vec.v[2], 0.0F}
        {
        }

        explicit Vector4(Color color);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& x() { return v[0]; }
        inline T x() const { return v[0]; }

        inline T& y() { return v[1]; }
        inline T y() const { return v[1]; }

        inline T& z() { return v[2]; }
        inline T z() const { return v[2]; }

        inline T& w() { return v[3]; }
        inline T w() const { return v[3]; }

        bool isZero() const
        {
            return v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 0;
        }

        float getAngle(const Vector4& axis) const
        {
            float dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            float dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            float dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return atan2f(sqrtf(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector4& min, const Vector4& max)
        {
            assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2] || min.v[3] > max.v[3]));

            // clamp the v[0] value
            if (v[0] < min.v[0])
                v[0] = min.v[0];
            if (v[0] > max.v[0])
                v[0] = max.v[0];

            // clamp the v[1] value
            if (v[1] < min.v[1])
                v[1] = min.v[1];
            if (v[1] > max.v[1])
                v[1] = max.v[1];

            // clamp the v[2] value
            if (v[2] < min.v[2])
                v[2] = min.v[2];
            if (v[2] > max.v[2])
                v[2] = max.v[2];

            // clamp the v[2] value
            if (v[3] < min.v[3])
                v[3] = min.v[3];
            if (v[3] > max.v[3])
                v[3] = max.v[3];
        }

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

        T dot(const Vector4& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3] * vec.v[3];
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

        void normalize()
        {
            float n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return;

            n = 1.0F / n;
            v[0] *= n;
            v[1] *= n;
            v[2] *= n;
            v[3] *= n;
        }

        void scale(const Vector4& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
            v[3] *= scale.v[3];
        }

        void smooth(const Vector4& target, float elapsedTime, float responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const
        {
            return std::min(v[0], std::min(v[1], std::min(v[2], v[3])));
        }

        T getMax() const
        {
            return std::max(v[0], std::max(v[1], std::max(v[2], v[3])));
        }

        inline const Vector4 operator+(const Vector4& vec) const
        {
            return Vector4(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2], v[3] + vec.v[3]);
        }

        inline Vector4& operator+=(const Vector4& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            v[3] += vec.v[3];
            return *this;
        }

        inline const Vector4 operator-(const Vector4& vec) const
        {
            return Vector4(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2], v[3] - vec.v[3]);
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

        inline const Vector4 operator*(T scalar) const
        {
            Vector4 result(*this);
            result *= scalar;
            return result;
        }

        inline Vector4& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;
            return *this;
        }

        inline const Vector4 operator/(T scalar) const
        {
            return Vector4(v[0] / scalar, v[1] / scalar, v[2] / scalar, v[3] / scalar);
        }

        inline Vector4& operator/=(T scalar)
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

    template<class T>
    inline const Vector4<T> operator*(T scalar, const Vector4<T>& vec)
    {
        return Vector4<T>(vec.v[0] * scalar, vec.v[1] * scalar, vec.v[2] * scalar, vec.v[3] * scalar);
    }

    using Vector4F = Vector4<float>;
}

#endif // OUZEL_MATH_VECTOR4_HPP
