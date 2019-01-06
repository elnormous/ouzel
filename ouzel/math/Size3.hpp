// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE3_HPP
#define OUZEL_MATH_SIZE3_HPP

#include <cstddef>
#include "math/Vector3.hpp"

namespace ouzel
{
    template<class T> class Size2;

    template<class T> class Size3 final
    {
    public:
        T v[3]{0, 0, 0};

        Size3()
        {
        }

        Size3(T width, T height, T depth):
            v{width, height, depth}
        {
        }

        explicit Size3(const Vector3<T>& point):
            v{point.v[0], point.v[1], point.v[2]}
        {
        }

        explicit Size3(const Size2<T>& s);

        inline T& operator[](size_t index) { return v[index]; }
        inline T operator[](size_t index) const { return v[index]; }

        inline T& width() { return v[0]; }
        inline T width() const { return v[0]; }

        inline T& height() { return v[1]; }
        inline T height() const { return v[1]; }

        inline T& depth() { return v[2]; }
        inline T depth() const { return v[2]; }

        inline const Size3 operator+(const Size3& size) const
        {
            return Size3(v[0] + size.v[0], v[1] + size.v[1], v[2] + size.v[2]);
        }

        inline Size3& operator+=(const Size3& size)
        {
            v[0] += size.v[0];
            v[1] += size.v[1];
            v[2] += size.v[2];
            return *this;
        }

        inline const Size3 operator-(const Size3& size) const
        {
            return Size3(v[0] - size.v[0], v[1] - size.v[1], v[2] - size.v[2]);
        }

        inline Size3& operator-=(const Size3& size)
        {
            v[0] -= size.v[0];
            v[1] -= size.v[1];
            v[2] -= size.v[2];
            return *this;
        }

        inline const Size3 operator*(T scalar) const
        {
            return Size3(v[0] * scalar, v[1] * scalar, v[2] * scalar);
        }

        inline Size3& operator*=(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            return *this;
        }

        inline const Size3 operator/(T scalar) const
        {
            return Size3(v[0] / scalar, v[1] / scalar, v[2] / scalar);
        }

        inline Size3& operator/=(T scalar)
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            return *this;
        }

        inline bool operator==(const Size3& size) const
        {
            return v[0] == size.v[0] && v[1] == size.v[1] && v[2] == size.v[2];
        }

        inline bool operator!=(const Size3& size) const
        {
            return v[0] != size.v[0] || v[1] != size.v[1] || v[2] != size.v[2];
        }

        inline bool isZero() const
        {
            return v[0] == 0 && v[1] == 0 && v[2] == 0;
        }

        inline T volume() const
        {
            return v[0] * v[1] * v[2];
        }

        inline void scale(T scalar)
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
        }

        inline void scale(const Vector3<T>& scale)
        {
            v[0] *= scale.v[0];
            v[1] *= scale.v[1];
            v[2] *= scale.v[2];
        }
    };

    template<class T>
    inline const Size3<T> operator*(const Size3<T>& size, const Vector3<T>& v)
    {
        return Size3<T>(size.v[0] * v.v[0], size.v[1] * v.v[1], size.v[2] * v.v[2]);
    }

    template<class T>
    inline const Size3<T> operator/(const Size3<T>& size, const Vector3<T>& v)
    {
        return Size3<T>(size.v[0] / v.v[0], size.v[1] / v.v[1], size.v[2] / v.v[2]);
    }
}

#endif // OUZEL_MATH_SIZE3_HPP
