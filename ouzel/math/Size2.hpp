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
        T width = 0;
        T height = 0;

        Size2()
        {
        }

        Size2(T initWidth, T initHeight):
            width(initWidth), height(initHeight)
        {
        }

        explicit Size2(const Vector2<T>& point):
            width(point.v[0]), height(point.v[1])
        {
        }

        inline Size2& operator=(const Vector2<T>& point)
        {
            width = point.v[0];
            height = point.v[1];
            return *this;
        }

        explicit Size2(const Size3<T>& s);
        Size2& operator=(const Size3<T>& s);

        inline const Size2 operator+(const Size2& size) const
        {
            return Size2(width + size.width, height + size.height);
        }

        inline Size2& operator+=(const Size2& size)
        {
            width += size.width;
            height += size.height;
            return *this;
        }

        inline const Size2 operator-(const Size2& size) const
        {
            return Size2(width - size.width, height - size.height);
        }

        inline Size2& operator-=(const Size2& size)
        {
            width -= size.width;
            height -= size.height;
            return *this;
        }

        inline const Size2 operator*(T scalar) const
        {
            return Size2(width * scalar, height * scalar);
        }

        inline Size2& operator*=(T scalar)
        {
            width *= scalar;
            height *= scalar;
            return *this;
        }

        inline const Size2 operator/(T scalar) const
        {
            return Size2(width / scalar, height / scalar);
        }

        inline Size2& operator/=(T scalar)
        {
            width /= scalar;
            height /= scalar;
            return *this;
        }

        inline bool operator==(const Size2& size) const
        {
            return width == size.width && height == size.height;
        }

        inline bool operator!=(const Size2& size) const
        {
            return width != size.width || height != size.height;
        }

        inline bool isZero() const
        {
            return width == 0 && height == 0;
        }

        inline T area() const
        {
            return width * height;
        }

        inline void scale(T scalar)
        {
            width *= scalar;
            height *= scalar;
        }

        inline void scale(const Vector2<T>& scale)
        {
            width *= scale.v[0];
            height *= scale.v[1];
        }
    };

    template<class T>
    inline const Size2<T> operator*(const Size2<T>& size, const Vector2<T>& v)
    {
        return Size2<T>(size.width * v.v[0], size.height * v.v[1]);
    }

    template<class T>
    inline const Size2<T> operator/(const Size2<T>& size, const Vector2<T>& v)
    {
        return Size2<T>(size.width / v.v[0], size.height / v.v[1]);
    }
}

#endif // OUZEL_MATH_SIZE2_HPP
