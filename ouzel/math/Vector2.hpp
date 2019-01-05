// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR2_HPP
#define OUZEL_MATH_VECTOR2_HPP

#include <cmath>

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

        Vector2& operator=(const Vector3<T>& vec);

        explicit Vector2(const Vector4<T>& vec);

        Vector2& operator=(const Vector4<T>& vec);

        inline T& x() { return v[0]; }
        inline T& y() { return v[1]; }

        inline T x() const { return v[0]; }
        inline T y() const { return v[1]; }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0;
        }

        static float angle(const Vector2& v1, const Vector2& v2)
        {
            float dz = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];
            return atan2f(fabsf(dz), dot(v1, v2));
        }

        void add(const Vector2& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
        }

        static void add(const Vector2& v1, const Vector2& v2, Vector2& dst)
        {
            dst.v[0] = v1.v[0] + v2.v[0];
            dst.v[1] = v1.v[1] + v2.v[1];
        }

        void clamp(const Vector2& min, const Vector2& max);

        static void clamp(const Vector2& vec, const Vector2& min, const Vector2& max, Vector2& dst);

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

        static T dot(const Vector2& v1, const Vector2& v2)
        {
            return v1.v[0] * v2.v[0] + v1.v[1] * v2.v[1];
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

        void normalize();

        void scale(T scalar)
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

        void set(T x, T y)
        {
            v[0] = x;
            v[1] = y;
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

        void smooth(const Vector2& target, float elapsedTime, float responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const;
        T getMax() const;

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

        inline float getAngle() const
        {
            return atan2f(v[1], v[0]);
        };
    };

    template<class T>
    inline const Vector2<T> operator*(T scalar, const Vector2<T>& vec)
    {
        return Vector2<T>(vec.v[0] * scalar, vec.v[1] * scalar);
    }
}

#endif // OUZEL_MATH_VECTOR2_HPP
