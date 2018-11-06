// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_SIZE3_HPP
#define OUZEL_SIZE3_HPP

#include "math/Vector3.hpp"

namespace ouzel
{
    class Size2;

    class Size3 final
    {
    public:
        float width = 0.0F;
        float height = 0.0F;
        float depth = 0.0F;

        Size3()
        {
        }

        Size3(float initWidth, float initHeight, float initDepth):
            width(initWidth), height(initHeight), depth(initDepth)
        {
        }

        explicit Size3(const Vector3& point):
            width(point.x), height(point.y), depth(point.z)
        {
        }

        inline Size3& operator=(const Vector3& point)
        {
            width = point.x;
            height = point.y;
            depth = point.z;
            return *this;
        }

        explicit Size3(const Size2& s);
        Size3& operator=(const Size2& s);

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

        inline const Size3 operator*(float a) const
        {
            return Size3(width * a, height * a, depth * a);
        }

        inline Size3& operator*=(float scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
            return *this;
        }

        inline const Size3 operator/(float a) const
        {
            return Size3(width / a, height / a, depth / a);
        }

        inline Size3& operator/=(float scalar)
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
            return width == 0.0F && height == 0.0F && depth == 0.0F;
        }

        inline bool isOne() const
        {
            return width == 1.0F && height == 1.0F && depth == 1.0F;
        }

        inline float volume() const
        {
            return width * height * depth;
        }

        inline void scale(float scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
        }

        inline void scale(const Vector3& scale)
        {
            width *= scale.x;
            height *= scale.y;
            depth *= scale.z;
        }
    };

    inline const Size3 operator*(const Size3& size, const Vector3& v)
    {
        return Size3(size.width * v.x, size.height * v.y, size.depth * v.z);
    }

    inline const Size3 operator/(const Size3& size, const Vector3& v)
    {
        return Size3(size.width / v.x, size.height / v.y, size.depth / v.z);
    }
}

#endif // OUZEL_SIZE3_HPP
