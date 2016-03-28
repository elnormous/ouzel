// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector3.h"

namespace ouzel
{
    class Size3
    {
    public:
        float width;
        float height;
        float depth;

        Size3();
        Size3(float w, float h, float d);

        Size3& operator=(const Vector3& point);
        Size3 operator+(const Size3& right) const;
        Size3 operator-(const Size3& right) const;
        Size3 operator*(float a) const;
        Size3 operator/(float a) const;

        inline bool operator==(const Size3& right) const
        {
            return width == right.width && height == right.height && depth == right.depth;
        }

        inline bool operator!=(const Size3& right) const
        {
            return width != right.width || height != right.height || depth != right.depth;
        }

        bool isZero() const;
    };
}
