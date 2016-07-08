// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector3.h"

namespace ouzel
{
    class Size2;

    class Size3
    {
    public:
        float width;
        float height;
        float depth;

        Size3():
            width(0.0f), height(0.0f), depth(0.0f)
        {
        }

        Size3(float w, float h, float d):
            width(w), height(h), depth(d)
        {
        }

        Size3(const Vector3& point):
            width(point.x),
            height(point.y),
            depth(point.z)
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
        Size3& operator=(const Size2& other);

        Size3 operator+(const Size3& right) const
        {
            return Size3(width + right.width, height + right.height, depth + right.depth);
        }

        Size3 operator-(const Size3& right) const
        {
            return Size3(width - right.width, height - right.height, depth - right.depth);
        }

        Size3 operator*(float a) const
        {
            return Size3(width * a, height * a, depth * a);
        }

        Size3 operator/(float a) const
        {
            return Size3(width / a, height / a, depth / a);
        }

        inline bool operator==(const Size3& right) const
        {
            return width == right.width && height == right.height && depth == right.depth;
        }

        inline bool operator!=(const Size3& right) const
        {
            return width != right.width || height != right.height || depth != right.depth;
        }

        bool isZero() const
        {
            return width == 0.0f && height == 0.0f && depth == 0.0f;
        }

        bool isOne() const
        {
            return width == 1.0f && height == 1.0f && depth == 1.0f;
        }
    };
}
