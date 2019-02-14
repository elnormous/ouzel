// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR2_HPP
#define OUZEL_MATH_VECTOR2_HPP

#include <cassert>
#include <cmath>
#include <cstddef>
#include <limits>

namespace ouzel
{
    template<class T> class Vector3;
    template<class T> class Vector4;

    template<class T> class Vector2 final
    {
    public:
        T v[2]{0, 0};

        Vector2()
        {
        }

        Vector2(T x, T y):
            v{x, y}
        {
        }

        Vector2(const Vector2<T>& copy):
            v{copy.v[0], copy.v[1]}
        {
        }

        Vector2& operator=(const Vector2<T>& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            return *this;
        }

        explicit Vector2(const Vector3<T>& vec);

        explicit Vector2(const Vector4<T>& vec);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& x() { return v[0]; }
        inline T x() const { return v[0]; }

        inline T& y() { return v[1]; }
        inline T y() const { return v[1]; }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0;
        }

        inline float getAngle() const
        {
            return atan2f(v[1], v[0]);
        };

        void clamp(const Vector2& min, const Vector2& max)
        {
            assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1]));

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
        }

        float distance(const Vector2& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];

            return sqrtf(dx * dx + dy * dy);
        }

        float distanceSquared(const Vector2& vec) const
        {
            float dx = vec.v[0] - v[0];
            float dy = vec.v[1] - v[1];
            return dx * dx + dy * dy;
        }

        T dot(const Vector2& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1];
        }

        float length() const
        {
            return sqrtf(v[0] * v[0] + v[1] * v[1]);
        }

        float lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1];
        }

        void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
        }

        void normalize()
        {
            float n = v[0] * v[0] + v[1] * v[1];
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < std::numeric_limits<float>::min()) // too close to zero
                return;

            n = 1.0F / n;
            v[0] *= n;
            v[1] *= n;
        }

        void scale(const Vector2& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
        }

        void rotate(float angle)
        {
            float sinAngle = sinf(angle);
            float cosAngle = cosf(angle);

            float tempX = v[0] * cosAngle - v[1] * sinAngle;
            v[1] = v[1] * cosAngle + v[0] * sinAngle;
            v[0] = tempX;
        }

        void rotate(const Vector2& point, float angle)
        {
            float sinAngle = sinf(angle);
            float cosAngle = cosf(angle);

            if (point.isZero())
            {
                float tempX = v[0] * cosAngle - v[1] * sinAngle;
                v[1] = v[1] * cosAngle + v[0] * sinAngle;
                v[0] = tempX;
            }
            else
            {
                float tempX = v[0] - point.v[0];
                float tempY = v[1] - point.v[1];

                v[0] = tempX * cosAngle - tempY * sinAngle + point.v[0];
                v[1] = tempY * cosAngle + tempX * sinAngle + point.v[1];
            }
        }

        void smooth(const Vector2& target, float elapsedTime, float responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const
        {
            return std::min(v[0], v[1]);
        }

        T getMax() const
        {
            return std::max(v[0], v[1]);
        }

        inline const Vector2 operator+(const Vector2& vec) const
        {
            return Vector2(v[0] + vec.v[0], v[1] + vec.v[1]);
        }

        inline Vector2& operator+=(const Vector2& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            return *this;
        }

        inline const Vector2 operator-(const Vector2& vec) const
        {
            return Vector2(v[0] - vec.v[0], v[1] - vec.v[1]);
        }

        inline Vector2& operator-=(const Vector2& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            return *this;
        }

        inline const Vector2 operator-() const
        {
            return Vector2(-v[0], -v[1]);
        }

        inline const Vector2 operator*(T scalar) const
        {
            return Vector2(v[0] * scalar, v[1] * scalar);
        }

        inline Vector2& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            return *this;
        }

        inline const Vector2 operator/(T scalar) const
        {
            return Vector2(v[0] / scalar, v[1] / scalar);
        }

        inline Vector2& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            return *this;
        }

        inline bool operator<(const Vector2& vec) const
        {
            if (v[0] == vec.v[0])
                return v[1] < vec.v[1];

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
    };

    template<class T>
    inline const Vector2<T> operator*(T scalar, const Vector2<T>& vec)
    {
        return Vector2<T>(vec.v[0] * scalar, vec.v[1] * scalar);
    }

    using Vector2F = Vector2<float>;
}

#endif // OUZEL_MATH_VECTOR2_HPP
