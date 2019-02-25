// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_VECTOR_HPP
#define OUZEL_MATH_VECTOR_HPP

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
        static constexpr size_t N = 2;
        T v[N]{0, 0};

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

        inline T getAngle() const
        {
            return atan2(v[1], v[0]);
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

        T distance(const Vector2& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];

            return sqrt(dx * dx + dy * dy);
        }

        T distanceSquared(const Vector2& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];
            return dx * dx + dy * dy;
        }

        T dot(const Vector2& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1];
        }

        T length() const
        {
            return sqrt(v[0] * v[0] + v[1] * v[1]);
        }

        T lengthSquared() const
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
            T n = v[0] * v[0] + v[1] * v[1];
            if (n == 1) // already normalized
                return;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return;

            n = 1 / n;
            v[0] *= n;
            v[1] *= n;
        }

        void scale(const Vector2& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
        }

        void rotate(T angle)
        {
            T sinAngle = sin(angle);
            T cosAngle = cos(angle);

            T tempX = v[0] * cosAngle - v[1] * sinAngle;
            v[1] = v[1] * cosAngle + v[0] * sinAngle;
            v[0] = tempX;
        }

        void rotate(const Vector2& point, T angle)
        {
            T sinAngle = sin(angle);
            T cosAngle = cos(angle);

            if (point.isZero())
            {
                T tempX = v[0] * cosAngle - v[1] * sinAngle;
                v[1] = v[1] * cosAngle + v[0] * sinAngle;
                v[0] = tempX;
            }
            else
            {
                T tempX = v[0] - point.v[0];
                T tempY = v[1] - point.v[1];

                v[0] = tempX * cosAngle - tempY * sinAngle + point.v[0];
                v[1] = tempY * cosAngle + tempX * sinAngle + point.v[1];
            }
        }

        void smooth(const Vector2& target, T elapsedTime, T responseTime)
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

    class Color;

    template<class T> class Vector3 final
    {
    public:
        static constexpr size_t N = 3;
        T v[N]{0, 0, 0};

        Vector3()
        {
        }

        Vector3(T x, T y):
            v{x, y, 0}
        {
        }

        Vector3(T x, T y, T z):
            v{x, y, z}
        {
        }

        Vector3(const Vector3& copy):
            v{copy.v[0], copy.v[1], copy.v[2]}
        {
        }

        Vector3& operator=(const Vector3& vec)
        {
            v[0] = vec.v[0];
            v[1] = vec.v[1];
            v[2] = vec.v[2];
            return *this;
        }

        explicit Vector3(const Vector2<T>& vec):
            v{vec.v[0], vec.v[1], 0}
        {
        }

        explicit Vector3(const Vector4<T>& vec);

        explicit Vector3(Color color);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& x() { return v[0]; }
        inline T x() const { return v[0]; }

        inline T& y() { return v[1]; }
        inline T y() const { return v[1]; }

        inline T& z() { return v[2]; }
        inline T z() const { return v[2]; }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0 && v[2] == 0;
        }

        T getAngle(const Vector3& axis) const
        {
            T dx = v[1] * axis.v[2] - v[2] * axis.v[1];
            T dy = v[2] * axis.v[0] - v[0] * axis.v[2];
            T dz = v[0] * axis.v[1] - v[1] * axis.v[0];

            return atan2(sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector3& min, const Vector3& max)
        {
            assert(!(min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2]));

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
        }

        Vector3 cross(const Vector3& vec) const
        {
            return Vector3((v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                           (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                           (v[0] * vec.v[1]) - (v[1] * vec.v[0]));
        }

        T distance(const Vector3& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];
            T dz = vec.v[2] - v[2];

            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        T distanceSquared(const Vector3& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];
            T dz = vec.v[2] - v[2];

            return dx * dx + dy * dy + dz * dz;
        }

        T dot(const Vector3& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2];
        }

        T length() const
        {
            return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }

        T lengthSquared() const
        {
            return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        }

        void negate()
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

        void normalize()
        {
            T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
            if (n == 1) // already normalized
                return;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return;

            n = 1 / n;
            v[0] *= n;
            v[1] *= n;
            v[2] *= n;
        }

        void scale(const Vector3& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
        }

        void smooth(const Vector3& target, T elapsedTime, T responseTime)
        {
            if (elapsedTime > 0)
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const
        {
            return std::min(v[0], std::min(v[1], v[2]));
        }

        T getMax() const
        {
            return std::max(v[0], std::max(v[1], v[2]));
        }

        inline const Vector3 operator+(const Vector3& vec) const
        {
            return Vector3(v[0] + vec.v[0], v[1] + vec.v[1], v[2] + vec.v[2]);
        }

        inline Vector3& operator+=(const Vector3& vec)
        {
            v[0] += vec.v[0];
            v[1] += vec.v[1];
            v[2] += vec.v[2];
            return *this;
        }

        inline const Vector3 operator-(const Vector3& vec) const
        {
            return Vector3(v[0] - vec.v[0], v[1] - vec.v[1], v[2] - vec.v[2]);
        }

        inline Vector3& operator-=(const Vector3& vec)
        {
            v[0] -= vec.v[0];
            v[1] -= vec.v[1];
            v[2] -= vec.v[2];
            return *this;
        }

        inline const Vector3 operator-() const
        {
            return Vector3(-v[0], -v[1], -v[2]);
        }

        inline const Vector3 operator*(T scalar) const
        {
            return Vector3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
        }

        inline Vector3& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            return *this;
        }

        inline const Vector3 operator/(T scalar) const
        {
            return Vector3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
        }

        inline Vector3& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            return *this;
        }

        inline bool operator<(const Vector3& vec) const
        {
            if (v[0] == vec.v[0])
            {
                if (v[1] == vec.v[1])
                    return v[2] < vec.v[2];

                return v[1] < vec.v[1];
            }
            return v[0] < vec.v[0];
        }

        inline bool operator==(const Vector3& vec) const
        {
            return v[0] == vec.v[0] && v[1] == vec.v[1] && v[2] == vec.v[2];
        }

        inline bool operator!=(const Vector3& vec) const
        {
            return v[0] != vec.v[0] || v[1] != vec.v[1] || v[2] != vec.v[2];
        }
    };

    template<class T>
    inline const Vector3<T> operator*(T scalar, const Vector3<T>& vec)
    {
        return Vector3<T>(vec.v[0] * scalar, vec.v[1] * scalar, vec.v[2] * scalar);
    }

    using Vector3F = Vector3<float>;

    template<class T> class Vector4 final
    {
    public:
        static constexpr size_t N = 4;
#if defined(__SSE__)
        alignas(16)
#endif
        T v[N]{0, 0, 0, 0};

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
            v{vec.v[0], vec.v[1], 0, 0}
        {
        }

        explicit Vector4(const Vector3<T>& vec):
            v{vec.v[0], vec.v[1], vec.v[2], 0}
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

        T getAngle(const Vector4& axis) const
        {
            T dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            T dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            T dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return atan2(sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
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

        T distance(const Vector4& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];
            T dz = vec.v[2] - v[2];
            T dw = vec.v[3] - v[3];

            return sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
        }

        T distanceSquared(const Vector4& vec) const
        {
            T dx = vec.v[0] - v[0];
            T dy = vec.v[1] - v[1];
            T dz = vec.v[2] - v[2];
            T dw = vec.v[3] - v[3];

            return dx * dx + dy * dy + dz * dz + dw * dw;
        }

        T dot(const Vector4& vec) const
        {
            return v[0] * vec.v[0] + v[1] * vec.v[1] + v[2] * vec.v[2] + v[3] * vec.v[3];
        }

        inline T length() const
        {
            return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
        }

        inline T lengthSquared() const
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
            T n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == 1) // already normalized
                return;

            n = sqrt(n);
            if (n <= std::numeric_limits<T>::min()) // too close to zero
                return;

            n = 1 / n;
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

        void smooth(const Vector4& target, T elapsedTime, T responseTime)
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

#endif // OUZEL_MATH_VECTOR_HPP
