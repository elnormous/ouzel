// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Vector2.h"

namespace ouzel
{
    class Size2
    {
    public:
        float width;
        float height;

        Size2();
        Size2(float width, float height);

        Size2& operator=(const Vector2& point);
        Size2 operator+(const Size2& right) const;
        Size2 operator-(const Size2& right) const;
        Size2 operator*(float a) const;
        Size2 operator/(float a) const;

        inline bool operator==(const Size2& right) const
        {
            return width == right.width && height == right.height;
        }

        inline bool operator!=(const Size2& right) const
        {
            return width != right.width || height != right.height;
        }

        bool isZero() const;
    };
}
