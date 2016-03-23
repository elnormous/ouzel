// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Size3.h"

namespace ouzel
{
    Size3::Size3():
        width(0.0f), height(0.0f), depth(0.0f)
    {

    }

    Size3::Size3(float width, float height, float depth):
        width(width), height(height), depth(depth)
    {

    }

    Size3& Size3::operator=(const Vector3& point)
    {
        width = point.x;
        height = point.y;
        depth = point.z;

        return *this;
    }

    Size3 Size3::operator+(const Size3& right) const
    {
        return Size3(width + right.width, height + right.height, depth + right.depth);
    }

    Size3 Size3::operator-(const Size3& right) const
    {
        return Size3(width - right.width, height - right.height, depth - right.depth);
    }

    Size3 Size3::operator*(float a) const
    {
        return Size3(width * a, height * a, depth * a);
    }

    Size3 Size3::operator/(float a) const
    {
        return Size3(width / a, height / a, depth / a);
    }

    bool Size3::isZero() const
    {
        return width == 0.0f && height == 0.0f && depth == 0.0f;
    }
}
