// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE2_HPP
#define OUZEL_MATH_SIZE2_HPP

#include "math/Vector2.hpp"

namespace ouzel
{
    template<class T> class Size3;

    template<class T> class Size2 final
    {
    public:
        T v[2]{0, 0};

        Size2()
        {
        }

        Size2(T width, T height):
            v{width, height}
        {
        }

        explicit Size2(const Vector2<T>& point):
            v{point.v[0], point.v[1]}
        {
        }

        inline Size2& operator=(const Vector2<T>& point)
        {
            v[0] = point.v[0];
            v[1] = point.v[1];
            return *this;
        }

        explicit Size2(const Size3<T>& s);
        Size2& operator=(const Size3<T>& s);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& width() { return v[0]; }
        inline T width() const { return v[0]; }

        inline T& height() { return v[1]; }
        inline T height() const { return v[1]; }

        inline const Size2 operator+(const Size2& size) const
        {
            return Size2(v[0] + size.v[0], v[1] + size.v[1]);
        }

        inline Size2& operator+=(const Size2& size)
        {
            v[0] += size.v[0];
            v[1] += size.v[1];
            return *this;
        }

        inline const Size2 operator-(const Size2& size) const
        {
            return Size2(v[0] - size.v[0], v[1] - size.v[1]);
        }

        inline Size2& operator-=(const Size2& size)
        {
            v[0] -= size.v[0];
            v[1] -= size.v[1];
            return *this;
        }

        inline const Size2 operator*(T scalar) const
        {
            return Size2(v[0] * scalar, v[1] * scalar);
        }

        inline Size2& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            return *this;
        }

        inline const Size2 operator/(T scalar) const
        {
            return Size2(v[0] / scalar, v[1] / scalar);
        }

        inline Size2& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            return *this;
        }

        inline bool operator==(const Size2& size) const
        {
            return v[0] == size.v[0] && v[1] == size.v[1];
        }

        inline bool operator!=(const Size2& size) const
        {
            return v[0] != size.v[0] || v[1] != size.v[1];
        }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0;
        }

        inline T area() const
        {
            return v[0] * v[1];
        }

        inline void scale(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
        }

        inline void scale(const Vector2<T>& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
        }
    };

    template<class T>
    inline const Size2<T> operator*(const Size2<T>& size, const Vector2<T>& v)
    {
        return Size2<T>(size.v[0] * v.v[0], size.v[1] * v.v[1]);
    }

    template<class T>
    inline const Size2<T> operator/(const Size2<T>& size, const Vector2<T>& v)
    {
        return Size2<T>(size.v[0] / v.v[0], size.v[1] / v.v[1]);
    }
}

#endif // OUZEL_MATH_SIZE2_HPP
