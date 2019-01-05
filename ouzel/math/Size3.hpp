// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MATH_SIZE3_HPP
#define OUZEL_MATH_SIZE3_HPP

#include "math/Vector3.hpp"

namespace ouzel
{
    template<class T> class Size2;

    template<class T> class Size3 final
    {
    public:
        T width = 0;
        T height = 0;
        T depth = 0;

        Size3()
        {
        }

        Size3(T initWidth, T initHeight, T initDepth):
            width(initWidth), height(initHeight), depth(initDepth)
        {
        }

        explicit Size3(const Vector3<T>& point):
            width(point.v[0]), height(point.v[1]), depth(point.v[2])
        {
        }

        inline Size3& operator=(const Vector3<T>& point)
        {
            width = point.v[0];
            height = point.v[1];
            depth = point.v[2];
            return *this;
        }

        explicit Size3(const Size2<T>& s);
        Size3& operator=(const Size2<T>& s);

        inline const Size3 operator+(const Size3& size) const
        {
            return Size3(width + size.width, height + size.height, depth + size.depth);
        }

        inline Size3& operator+=(const Size3& size)
        {
            width += size.width;
            height += size.height;
            depth += size.depth;
            return *this;
        }

        inline const Size3 operator-(const Size3& size) const
        {
            return Size3(width - size.width, height - size.height, depth - size.depth);
        }

        inline Size3& operator-=(const Size3& size)
        {
            width -= size.width;
            height -= size.height;
            depth -= size.depth;
            return *this;
        }

        inline const Size3 operator*(T scalar) const
        {
            return Size3(width * scalar, height * scalar, depth * scalar);
        }

        inline Size3& operator*=(T scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
            return *this;
        }

        inline const Size3 operator/(T scalar) const
        {
            return Size3(width / scalar, height / scalar, depth / scalar);
        }

        inline Size3& operator/=(T scalar)
        {
            width /= scalar;
            height /= scalar;
            depth /= scalar;
            return *this;
        }

        inline bool operator==(const Size3& size) const
        {
            return width == size.width && height == size.height && depth == size.depth;
        }

        inline bool operator!=(const Size3& size) const
        {
            return width != size.width || height != size.height || depth != size.depth;
        }

        inline bool isZero() const
        {
            return width == 0 && height == 0 && depth == 0;
        }

        inline T volume() const
        {
            return width * height * depth;
        }

        inline void scale(T scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
        }

        inline void scale(const Vector3<T>& scale)
        {
            width *= scale.v[0];
            height *= scale.v[1];
            depth *= scale.v[2];
        }
    };

    template<class T>
    inline const Size3<T> operator*(const Size3<T>& size, const Vector3<T>& v)
    {
        return Size3<T>(size.width * v.v[0], size.height * v.v[1], size.depth * v.v[2]);
    }

    template<class T>
    inline const Size3<T> operator/(const Size3<T>& size, const Vector3<T>& v)
    {
        return Size3<T>(size.width / v.v[0], size.height / v.v[1], size.depth / v.v[2]);
    }
}

#endif // OUZEL_MATH_SIZE3_HPP
