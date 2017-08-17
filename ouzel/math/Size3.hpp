// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "math/Vector3.hpp"

namespace ouzel
{
    class Size2;

    class Size3
    {
    public:
        float width = 0.0f;
        float height = 0.0f;
        float depth = 0.0f;

        Size3()
        {
        }

        Size3(float w, float h, float d):
            width(w), height(h), depth(d)
        {
        }

        Size3(const Vector3& point):
            width(point.x), height(point.y), depth(point.z)
        {
        }

        Size3& operator=(const Vector3& point)
        {
            width = point.x;
            height = point.y;
            depth = point.z;
            return *this;
        }

        Size3(const Size2& s);
        Size3& operator=(const Size2& s);

        Size3 operator+(const Size3& size) const
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

        Size3 operator-(const Size3& size) const
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

        Size3 operator*(float a) const
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

        Size3 operator/(float a) const
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

        bool isZero() const
        {
            return width == 0.0f && height == 0.0f && depth == 0.0f;
        }

        bool isOne() const
        {
            return width == 1.0f && height == 1.0f && depth == 1.0f;
        }

        float volume() const
        {
            return width * height * depth;
        }

        void scale(float scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
        }

        void scale(const Vector3& scale)
        {
            width *= scale.x;
            height *= scale.y;
            depth *= scale.z;
        }
    };

    inline Size3 operator*(const Size3& size, const Vector3& v)
    {
        return Size3(size.width * v.x, size.height * v.y, size.depth * v.z);
    }

    inline Size3 operator/(const Size3& size, const Vector3& v)
    {
        return Size3(size.width / v.x, size.height / v.y, size.depth / v.z);
    }
}
