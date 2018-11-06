// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "math/Vector2.hpp"

namespace ouzel
{
    class Size3;

    class Size2 final
    {
    public:
        float width = 0.0F;
        float height = 0.0F;

        Size2()
        {
        }

        Size2(float initWidth, float initHeight):
            width(initWidth), height(initHeight)
        {
        }

        explicit Size2(const Vector2& point):
            width(point.x), height(point.y)
        {
        }

        inline Size2& operator=(const Vector2& point)
        {
            width = point.x;
            height = point.y;
            return *this;
        }

        explicit Size2(const Size3& s);
        Size2& operator=(const Size3& s);

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

        inline const Size2 operator*(float a) const
        {
            return Size2(width * a, height * a);
        }

        inline Size2& operator*=(float scalar)
        {
            width *= scalar;
            height *= scalar;
            return *this;
        }

        inline const Size2 operator/(float a) const
        {
            return Size2(width / a, height / a);
        }

        inline Size2& operator/=(float scalar)
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
            return width == 0.0F && height == 0.0F;
        }

        inline bool isOne() const
        {
            return width == 1.0F && height == 1.0F;
        }

        inline float area() const
        {
            return width * height;
        }

        inline void scale(float scalar)
        {
            width *= scalar;
            height *= scalar;
        }

        inline void scale(const Vector2& scale)
        {
            width *= scale.x;
            height *= scale.y;
        }
    };

    inline const Size2 operator*(const Size2& size, const Vector2& v)
    {
        return Size2(size.width * v.x, size.height * v.y);
    }

    inline const Size2 operator/(const Size2& size, const Vector2& v)
    {
        return Size2(size.width / v.x, size.height / v.y);
    }
}
