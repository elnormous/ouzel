// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

namespace ouzel
{
    class Size3;

    class Size2
    {
    public:
        float width;
        float height;

        Size2():
            width(0.0f), height(0.0f)
        {
        }

        Size2(float w, float h):
            width(w), height(h)
        {
        }

        Size2(const Vector2& point):
            width(point.v[0]),
            height(point.v[1])
        {
        }

        Size2& operator=(const Vector2& point)
        {
            width = point.v[0];
            height = point.v[1];
            return *this;
        }

        Size2(const Size3& s);
        Size2& operator=(const Size3& size);

        Size2 operator+(const Size2& size) const
        {
            return Size2(width + size.width, height + size.height);
        }

        Size2 operator-(const Size2& size) const
        {
            return Size2(width - size.width, height - size.height);
        }

        Size2 operator*(float a) const
        {
            return Size2(width * a, height * a);
        }

        Size2 operator/(float a) const
        {
            return Size2(width / a, height / a);
        }

        inline bool operator==(const Size2& size) const
        {
            return width == size.width && height == size.height;
        }

        inline bool operator!=(const Size2& size) const
        {
            return width != size.width || height != size.height;
        }

        bool isZero() const
        {
            return width == 0.0f && height == 0.0f;
        }

        bool isOne() const
        {
            return width == 1.0f && height == 1.0f;
        }
    };

    inline Size2 operator*(const Size2& size, const Vector2& v)
    {
        return Size2(size.width * v.v[0], size.height * v.v[1]);
    }

    inline Size2 operator/(const Size2& size, const Vector2& v)
    {
        return Size2(size.width / v.v[0], size.height / v.v[1]);
    }
}
